#include "Core_PCH.h"
#include "BotImpl.h"

#include "CodingameUtility\Random.h"

#include "TreeEntity.h"

using namespace std;

namespace sc2021
{

    // Initialize

    STurnOutputData CBotImpl::FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData)
    {
        int const seed = UpdateRandomSeed();
        cerr << "Bot seed: " << seed << "\n";

        InitCells(initInData);

        auto& day0 = m_dayStrategies.emplace_back();
        day0.m_strategyType = EDayStrategyType::Predefined;
        day0.m_turnStrategies.push_back(CreateIncreaseIncomeTS(true));

        auto& day1 = m_dayStrategies.emplace_back();
        day1.m_strategyType = EDayStrategyType::Predefined;
        day1.m_turnStrategies.push_back(CreateIncreaseIncomeTS(true));

        auto& day2 = m_dayStrategies.emplace_back();
        day2.m_strategyType = EDayStrategyType::Predefined;
        day2.m_turnStrategies.push_back(CreateUpgradeToLargeTreeTS(true));

        auto& day3 = m_dayStrategies.emplace_back();
        day3.m_strategyType = EDayStrategyType::Predefined;
        day3.m_turnStrategies.push_back(CreateUpgradeToLargeTreeTS(true));

        auto& day4 = m_dayStrategies.emplace_back();
        day4.m_strategyType = EDayStrategyType::Predefined;
//        day4.m_turnStrategies.push_back(CreateCompleteLifeCycleTS(true));

        auto& day5 = m_dayStrategies.emplace_back();
        day5.m_strategyType = EDayStrategyType::Predefined;
        day5.m_turnStrategies.push_back(CreateCompleteLifeCycleTS(true));

        auto& defaultDay = m_dayStrategies.emplace_back();
        defaultDay.m_strategyType = EDayStrategyType::Predefined;
        defaultDay.m_turnStrategies.push_back(CreateCompleteLifeCycleTS(true));
        defaultDay.m_turnStrategies.push_back(CreateUpgradeToLargeTreeTS(true));

        m_turnData.m_day = -1;

        return Update(turnInData);
    }

    void CBotImpl::InitCells(SInitInputData const& initInData)
    {
        m_cells.resize(initInData.m_cells.size());
        for (auto const& cellData : initInData.m_cells)
        {
            auto& cell = m_cells[cellData.m_index];
            cell.m_index = cellData.m_index;
            cell.m_richness = cellData.m_richness;
            cell.m_tree.Invalidate();
        }
    }
    
    // Update

    STurnOutputData CBotImpl::Update(STurnInputData const& turnInData)
    {
        UpdateTrees(turnInData);

        if (turnInData.m_day != m_turnData.m_day)
        {
            OnNewDay(turnInData);
        }

        m_turnData = turnInData;

        STurnOutputData outputData;
        outputData.m_command = FindTurn();
        return outputData;
    }

    void CBotImpl::UpdateTrees(STurnInputData const& turnInData)
    {
        memset(m_myTreesCntBySize, 0, sizeof(m_myTreesCntBySize));

        CVectorInPlace<int, MAX_CELLS_COUNT> indicesWithTree;
        for (auto const& treeData : turnInData.m_trees)
        {
            indicesWithTree.push_back(treeData.m_cellIndex);
            auto& cell = m_cells[treeData.m_cellIndex];

            cell.m_tree.m_size = treeData.m_size;
            cell.m_tree.m_isMine = treeData.m_isMine;
            cell.m_tree.m_isDormant = treeData.m_isDormant;
            
            if (treeData.m_isMine)
            {
                ++m_myTreesCntBySize[treeData.m_size];
            }
        }

        for (auto& cell : m_cells)
        {
            if (find(indicesWithTree.begin(), indicesWithTree.end(), cell.m_index) == indicesWithTree.end())
            {
                cell.m_tree.Invalidate();
            }
        }
    }

    void CBotImpl::OnNewDay(STurnInputData const& newData)
    {
        if (newData.m_day >= m_dayStrategies.size())
        {
            m_currentDayStrategy = m_dayStrategies.back();
        }
        else
        {
            m_currentDayStrategy = m_dayStrategies[newData.m_day];
        }

        reverse(m_currentDayStrategy.m_turnStrategies.begin(), m_currentDayStrategy.m_turnStrategies.end());
    }

    // Find turn

    SCommand CBotImpl::FindTurn()
    {
        auto& turns = m_currentDayStrategy.m_turnStrategies;
        while (!turns.empty())
        {
            auto const& turn = turns.back();
            SCommand const cmd = FindTurnByStrategy(turn);

            if (!cmd.IsValid())
            {
                turns.pop_back();
            }
            else
            {
                if (!turn.m_repeat)
                {
                    turns.pop_back();
                }

                return cmd;
            }
        }

        return CreateWaitCmd();
    }


    SCommand CBotImpl::FindTurnByStrategy(STurnStrategy const turnStrategy)
    {
        switch (turnStrategy.m_strategyType)
        {
        case ETurnStrategyType::CompleteLifeCycle:
            return FindTurn_CompleteLifeCycle();
        case ETurnStrategyType::IncreaseIncome:
            return FindTurn_IncreaseIncome();
        case ETurnStrategyType::UpgradeToLargeTree:
            return FindTurn_UpgradeToLargeTree();
        default:
            assert(!"Unhandled ETurnStrategyType");
        }

        assert(false);
        return SCommand();
    }

    SCommand CBotImpl::FindTurn_CompleteLifeCycle()
    {
        cerr << "FindTurn_CompleteLifeCycle\n";
        if (m_turnData.m_mySun < COMPLETE_LIFECYCLE_PRICE)
        {
            return SCommand();
        }

        SCellEntity const* curCell = nullptr;
        for (auto const& cell : m_cells)
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
            return SCommand();
        }

        return CreateCompleteCmd(curCell->m_index);
    }

    SCommand CBotImpl::FindTurn_IncreaseIncome()
    {
        cerr << "FindTurn_IncreaseIncome\n";
        SCellEntity const* curCell = nullptr;
        int curUpgradePrice = -1;
        for (auto const& cell : m_cells)
        {
            if (cell.HasMyTree_Dormant(false) && (cell.m_tree.m_size != MAX_TREE_SIZE && cell.m_tree.m_size != 0))
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
            return SCommand();
        }

        return CreateGrowCmd(curCell->m_index);
    }

    SCommand CBotImpl::FindTurn_UpgradeToLargeTree()
    {
        cerr << "FindTurn_UpgradeToLargeTree\n";
        SCellEntity const* curCell = nullptr;
        for (auto const& cell : m_cells)
        {
            if (cell.HasMyTree_Dormant(false) && (cell.m_tree.m_size != MAX_TREE_SIZE && cell.m_tree.m_size != 0))
            {
                if (curCell == nullptr
                    || curCell->m_tree.m_size < cell.m_tree.m_size
                    || (curCell->m_tree.m_size == cell.m_tree.m_size && curCell->m_richness < cell.m_richness))
                {
                    curCell = &cell;
                }
            }
        }

        if (curCell == nullptr || GetMySun() < GetUpgradePrice(curCell->m_tree.m_size))
        {
            return SCommand();
        }

        return CreateGrowCmd(curCell->m_index);
    }
}