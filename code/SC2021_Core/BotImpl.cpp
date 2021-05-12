#include "Core_PCH.h"
#include "BotImpl.h"

#include "CodingameUtility\CompositeFunction.h"
#include "CodingameUtility\Profiler.h"
#include "CodingameUtility\Random.h"

#include "CommandHelper.h"
#include "StrategyHelper.h"
#include "TreeEntity.h"

using namespace std;

namespace BotConfigs
{
    // SM - shadow manager
    constexpr int SM_MAX_DAYS_TO_SIMULATE = 6;
}

namespace sc2021
{
    // Initialize
    STurnOutputData CBotImpl::FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData)
    {
        int const seed = UpdateRandomSeed();
        cerr << "Bot seed: " << seed << "\n";

        InitMap(initInData);

        // predefined strategies
        {
            int nxtDay = 0;

            {
                auto& day = m_predefinedDayStrategies[nxtDay++];
                day.Validate();
            }

            {
                auto& day = m_predefinedDayStrategies[nxtDay++];
                day.m_turnStrategies.push_back(CreateIncreaseIncomeTS());
            }

            {
                auto& day = m_predefinedDayStrategies[nxtDay++];
                day.m_turnStrategies.push_back(CreateSeedNewTreeTS());
                day.m_turnStrategies.push_back(CreateIncreaseIncomeTS());
            }

            {
                auto& preLastDay = m_predefinedDayStrategies[LAST_DAY_NUMBER - 1];
                preLastDay.m_turnStrategies.push_back(CreateCompleteLifeCycleTS(INFINITY_ITERATIONS_COUNT));
                preLastDay.m_turnStrategies.push_back(CreateUpgradeToLargeTreeTS(INFINITY_ITERATIONS_COUNT));
            }

            {
                auto& lastDay = m_predefinedDayStrategies[LAST_DAY_NUMBER];
                lastDay.m_turnStrategies.emplace_back(CreateCompleteLifeCycleTS(INFINITY_ITERATIONS_COUNT));
            }

            m_defaultDayStrategy.m_turnStrategies.push_back(
                CreateCompleteLifeCycleTS()
                    .SetCondition([this](auto const&) {
                        if (GetCurrentDay() < 15)
                            return m_myTreesCntBySize[MAX_TREE_SIZE] > 4;
                        if (GetCurrentDay() < 20)
                            return m_myTreesCntBySize[MAX_TREE_SIZE] > 3;
                        return m_myTreesCntBySize[MAX_TREE_SIZE] >= 2;
                    })
            );

            m_defaultDayStrategy.m_turnStrategies.push_back(
                CreateIncreaseIncomeTS(INFINITY_ITERATIONS_COUNT)
                    .SetCondition([this](auto const&) {
                        return GetCurrentDay() & 1;
                    })
            );

            m_defaultDayStrategy.m_turnStrategies.push_back(
                CreateUpgradeToLargeTreeTS(INFINITY_ITERATIONS_COUNT)
                    .SetCondition([this](auto const&) {
                        return !(GetCurrentDay() & 1);
                    })
            );

            m_defaultDayStrategy.m_turnStrategies.push_back(
                CreateSeedNewTreeTS()
                    .SetCondition([this](auto const&) {
                        return m_myTreesCntBySize[0] == 0 && ((GetCurrentDay() % 4) != 0);
                    })
            );
        }

        // to force a new day event on the first update
        m_turnData.m_day = -1;

        return Update(turnInData);
    }

    void CBotImpl::InitMap(SInitInputData const& initInData)
    {
        for (auto const& cellData : initInData.m_cells)
        {
            auto& cell = m_map[cellData.m_index];
            cell.m_index = cellData.m_index;
            cell.m_richness = cellData.m_richness;
            cell.m_tree.Invalidate();

            int direction = 0;
            for (int neighIndex : cellData.m_neighs)
            {
                if (neighIndex >= 0)
                {
                    cell.m_neigh[direction] = m_map.begin() + neighIndex;
                }
                ++direction;
            }
        }
    }
    
    // Update
    STurnOutputData CBotImpl::Update(STurnInputData const& turnInData)
    {
        bool const isNewDay = turnInData.m_day != m_turnData.m_day;
        m_turnData = turnInData;

        UpdateTrees();
        UpdateShadowManager();

        if (isNewDay)
        {
            OnNewDay();
        }

        STurnOutputData outputData;
        outputData.m_command = FindTurn();
        return outputData;
    }

    void CBotImpl::UpdateTrees()
    {
        memset(m_myTreesCntBySize, 0, sizeof(m_myTreesCntBySize));
        memset(m_oppTreesCntBySize, 0, sizeof(m_oppTreesCntBySize));

        CVectorInPlace<int, MAX_CELLS_COUNT> indicesWithTree;
        for (auto const& treeData : m_turnData.m_trees)
        {
            indicesWithTree.push_back(treeData.m_cellIndex);
            auto& cell = m_map[treeData.m_cellIndex];

            cell.m_tree.m_size = treeData.m_size;
            cell.m_tree.m_isMine = treeData.m_isMine;
            cell.m_tree.m_isDormant = treeData.m_isDormant;
            
            if (treeData.m_isMine)
            {
                ++m_myTreesCntBySize[treeData.m_size];
            }
            else
            {
                ++m_oppTreesCntBySize[treeData.m_size];
            }
        }

        for (auto& cell : m_map)
        {
            if (find(indicesWithTree.begin(), indicesWithTree.end(), cell.m_index) == indicesWithTree.end())
            {
                cell.m_tree.Invalidate();
            }
        }
    }

    void CBotImpl::UpdateShadowManager()
    {
        PROFILE_TIME("UpdateShadowManager");

        int const daysToSimulate = min(BotConfigs::SM_MAX_DAYS_TO_SIMULATE, GetDaysRemaining());
        m_shadowManager.UpdateDarknessLevel(m_map, GetCurrentShadowDirection(GetCurrentDay()), daysToSimulate);
    }

    // Events
    void CBotImpl::OnNewDay()
    {
        if (m_predefinedDayStrategies.find(GetCurrentDay()) == m_predefinedDayStrategies.end())
        {
            m_currentDayStrategy = CalculateDayStrategy();
            if (!m_currentDayStrategy.IsValid())
            {
                m_currentDayStrategy = m_defaultDayStrategy;
            }
        }
        else
        {
            m_currentDayStrategy = m_predefinedDayStrategies[GetCurrentDay()];
        }

        reverse(m_currentDayStrategy.m_turnStrategies.begin(), m_currentDayStrategy.m_turnStrategies.end());
    }

    // Day strategy
    SDayStrategy CBotImpl::CalculateDayStrategy()
    {
        static const CRanges gamePeriods{ 6 };
        size_t const gamePeriod = gamePeriods.GetRangeIndex(GetCurrentDay());

        SDayStrategy strategy = INVALID_DAY_STRATEGY;

        if (gamePeriod == 0)
        {
            strategy.m_turnStrategies.push_back(CreateUpgradeToLargeTreeTS());
            strategy.m_turnStrategies.push_back(CreateIncreaseIncomeTS());
        }
        else if (gamePeriod == 1)
        {
            static const CRanges largeTreeRanges{ 15, 20 };
            static const CVectorInPlace<int, 3> largeTreeMaxCounts{ 5, 4, 2 };
            assert(largeTreeMaxCounts.size() == largeTreeRanges.RangesCount());

            size_t const largeTreeRange = largeTreeRanges.GetRangeIndex(GetCurrentDay());
            int const largeTreeMaxCount = largeTreeMaxCounts[largeTreeRange];
            int const largeTreeCurrentCount = m_myTreesCntBySize[MAX_TREE_SIZE];
            {
                if (largeTreeCurrentCount >= largeTreeMaxCount)
                {
                    strategy.m_turnStrategies.push_back(CreateCompleteLifeCycleTS());
                }
                else if (largeTreeCurrentCount == largeTreeMaxCount - 1)
                {
                    bool const canUpgradeToLarge = ((GetUpgradePrice(2) - 1 + COMPLETE_LIFECYCLE_PRICE) <= GetMySun()) && (m_myTreesCntBySize[MAX_TREE_SIZE - 1] > 0);
                    if (canUpgradeToLarge)
                    {
                        strategy.m_turnStrategies.push_back(CreateCompleteLifeCycleTS());
                        strategy.m_turnStrategies.push_back(CreateUpgradeToLargeTreeTS());
                    }
                }
            }

            {
                auto const upgradeToLargeTreeTS = CreateUpgradeToLargeTreeTS()
                    .SetCondition(
                        [this, largeTreeMaxCount](auto const&) {
                            return m_myTreesCntBySize[MAX_TREE_SIZE] < largeTreeMaxCount;
                        }
                );
                auto const increaseIncomeTS = CreateIncreaseIncomeTS();

                strategy.m_turnStrategies.push_back(upgradeToLargeTreeTS);
                for (int i = 0; i < 4; ++i)
                {
                    if (GetCurrentDay() & 1)
                    {
                        strategy.m_turnStrategies.push_back(increaseIncomeTS);
                        strategy.m_turnStrategies.push_back(upgradeToLargeTreeTS);
                    }
                    else
                    {
                        strategy.m_turnStrategies.push_back(upgradeToLargeTreeTS);
                        strategy.m_turnStrategies.push_back(increaseIncomeTS);
                    }
                }
            }
        }

        strategy.m_turnStrategies.push_back(
            CreateSeedNewTreeTS()
            .SetCondition(
                [this](auto const&) {
                    return m_myTreesCntBySize[0] == 0;
                }
            )
        );

        return strategy;
    }

    // Find turn
    SCommand CBotImpl::FindTurn()
    {
        static int iterationsCount = 0;

        SCommand cmd;
        auto& turns = m_currentDayStrategy.m_turnStrategies;
        while (!turns.empty() && !cmd.IsValid())
        {
            ++iterationsCount;
            auto const& turn = turns.back();
            string logLine = ToString(turn);
            logLine += " Iteration: " + to_string(iterationsCount);            

            bool removeTurn = false;
            if (turn.m_condition && !turn.m_condition(turn))
            {
                logLine += " -> Skip";
                removeTurn = true;
            }
            else
            {
                cmd = FindTurnByStrategy(turn);
                if (!cmd.IsValid())
                {
                    logLine += " -> Fail";
                    removeTurn = true;
                }
                else
                {
                    logLine += " -> Success";
                    if (turn.m_iterationsCount != INFINITY_ITERATIONS_COUNT
                        && iterationsCount >= turn.m_iterationsCount)
                    {
                        removeTurn = true;
                    }
                }
            }

            cerr << logLine << "\n";
            if (removeTurn)
            {
                iterationsCount = 0;
                turns.pop_back();
            }
        }

        return cmd.IsValid() ? cmd : CreateWaitCmd();
    }


    SCommand CBotImpl::FindTurnByStrategy(STurnStrategy const& turnStrategy)
    {
        switch (turnStrategy.m_strategyType)
        {
        case ETurnStrategyType::CompleteLifeCycle:
            return FindTurn_CompleteLifeCycle(turnStrategy);
        case ETurnStrategyType::IncreaseIncome:
            return FindTurn_IncreaseIncome(turnStrategy);
        case ETurnStrategyType::UpgradeToLargeTree:
            return FindTurn_UpgradeToLargeTree(turnStrategy);
        case ETurnStrategyType::SeedNewTree:
            return FindTurn_SeedNewTree(turnStrategy);
        default:
            assert(!"Unhandled ETurnStrategyType");
        }

        assert(false);
        return INVALID_COMMAND;
    }

    SCommand CBotImpl::FindTurn_CompleteLifeCycle(STurnStrategy const& turnStrategy)
    {
        if (m_turnData.m_mySun < COMPLETE_LIFECYCLE_PRICE)
        {
            return INVALID_COMMAND;
        }

        SCellEntity const* curCell = nullptr;
        for (auto const& cell : m_map)
        {
            if (cell.HasMyTree_Dormant(false) && cell.m_tree.m_size == MAX_TREE_SIZE)
            {
                if (curCell == nullptr || curCell->m_richness < cell.m_richness)
                {
                    curCell = &cell;
                }
            }
        }

        if (!curCell || (curCell->m_richness <= 1 && m_turnData.m_nutriens == 0))
        {
            return INVALID_COMMAND;
        }

        return CreateCompleteCmd(curCell->m_index);
    }

    SCommand CBotImpl::FindTurn_IncreaseIncome(STurnStrategy const& turnStrategy)
    {
        SCellEntity const* curCell = nullptr;
        int curUpgradePrice = -1;
        for (auto const& cell : m_map)
        {
            if (cell.HasMyTree_Dormant(false) && cell.m_tree.m_size < MAX_TREE_SIZE)
            {
                int const price = GetUpgradePrice(cell.m_tree.m_size);
                if (curUpgradePrice == -1
                    || price < curUpgradePrice
                    || price == curUpgradePrice && curCell->m_richness < cell.m_richness)
                {
                    curUpgradePrice = price;
                    curCell = &cell;
                }
            }
        }

        if (curUpgradePrice == -1 || GetMySun() < curUpgradePrice)
        {
            return INVALID_COMMAND;
        }

        return CreateGrowCmd(curCell->m_index);
    }

    SCommand CBotImpl::FindTurn_UpgradeToLargeTree(STurnStrategy const& turnStrategy)
    {
        CMap::Cells validCells;
        m_map.GetCells(validCells, [this](SCellEntity const& cell)
            {
                return cell.HasMyTree_Dormant(false)
                    && cell.m_tree.m_size < MAX_TREE_SIZE
                    && (MAX_TREE_SIZE - cell.m_tree.m_size) <= GetDaysRemaining();
            }
        );

        SCellEntity const* curCell = nullptr;
        for (auto const& cell : validCells)
        {
            if (curCell == nullptr
                || curCell->m_tree.m_size < cell->m_tree.m_size
                || (curCell->m_tree.m_size == cell->m_tree.m_size && curCell->m_richness < cell->m_richness))
            {
                curCell = cell;
            }
        }

        // TODO: take into account available sun points. It's impractical to upgrade the tree to lvl 3 and don't have enough suns to complete it.

        if (curCell == nullptr || GetMySun() < GetUpgradePrice(curCell->m_tree.m_size))
        {
            return INVALID_COMMAND;
        }

        return CreateGrowCmd(curCell->m_index);
    }

    SCommand CBotImpl::FindTurn_SeedNewTree(STurnStrategy const& turnStrategy)
    {
        static const CRanges ranges{7, 16};

        if (GetMySun() < GetSeedPrice())
        {
            return INVALID_COMMAND;
        }

        size_t const gamePeriod = ranges.GetRangeIndex(GetCurrentDay());
        auto fitnessFunc = [this, gamePeriod](SCellEntity const* cell)
        {
            // TODO: take into account current number of large trees - nutries number will decrease
            float const expectedNutriensCount = (float)GetNutriens();// -(float)GetTreesCount(MAX_TREE_SIZE) * 0.33f;
            float const richnessScore = ((float)CELL_RICHNESS_SCORE[cell->m_richness] + expectedNutriensCount) / ((float)CELL_RICHNESS_SCORE[MAX_CELL_RICHNESS] + expectedNutriensCount);
            float const richnessCoef = (float)GetCurrentDay() / LAST_DAY_NUMBER;

            float const darknessScore = 1.0f - m_shadowManager.GetDarknessLevelInRange_Avrg(cell->m_index, 2, -1);
            float const darknessCoef = 1.0f - richnessCoef;

            return richnessScore * richnessCoef + darknessScore * darknessCoef;
        };

        SCellEntity const* curCell = nullptr;
        SCellEntity const* curSeedCell = nullptr;
        
        CMap::CellEntries neighCellEntries;
        for (auto const& cell : m_map)
        {
            if (cell.HasMyTree_Dormant(false) && cell.m_tree.m_size > 0)
            {
                m_map.GetCellsInRadius(neighCellEntries, cell.m_index, cell.m_tree.m_size);
                for (auto const entry : neighCellEntries)
                {
                    auto const neighCell = entry.m_cell;
                    if (neighCell->m_richness == 0) continue;
                    if (neighCell->HasTree()) continue;

                    if (curSeedCell == nullptr
                        || (fitnessFunc(curSeedCell) < fitnessFunc(neighCell)))
                    {
                        curCell = &cell;
                        curSeedCell = neighCell;
                    }
                }
            }
        }

        if (!curCell)
        {
            return INVALID_COMMAND;
        }

        assert(curCell && curSeedCell);
        return CreateSeedCmd(curCell->m_index, curSeedCell->m_index);
    }
}