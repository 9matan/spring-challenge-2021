#pragma once

#include "CellEntity.h"
#include "InputData.h"
#include "OutputData.h"
#include "Strategy.h"

// TODO: Introduce the map with an impact of the shadow on each cell

namespace sc2021
{
    class CBotImpl
    {
    public:
        STurnOutputData FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData);
        STurnOutputData Update(STurnInputData const& turnInData);
    private:
        STurnInputData m_turnData;
        std::unordered_map<int, SDayStrategy> m_predefinedDayStrategies;
        SDayStrategy m_defaultDayStrategy;

        SDayStrategy m_currentDayStrategy;
        CVectorInPlace<SCellEntity, MAX_CELLS_COUNT> m_cells;
        int m_myTreesCntBySize[MAX_TREE_SIZE + 1];
    private:
        void InitCells(SInitInputData const& initData);

        void UpdateTrees(STurnInputData const& turnInData);

        // Day strategy
        SDayStrategy CalculateDayStrategy();

        // Find turn
        SCommand FindTurn();
        SCommand FindTurnByStrategy(STurnStrategy const& turnStrategy);

        SCommand FindTurn_CompleteLifeCycle(STurnStrategy const& turnStrategy);
        SCommand FindTurn_IncreaseIncome(STurnStrategy const& turnStrategy);
        SCommand FindTurn_UpgradeToLargeTree(STurnStrategy const& turnStrategy);
        SCommand FindTurn_SeedNewTree(STurnStrategy const& turnStrategy);

        // Events
        void OnNewDay(STurnInputData const& newData);

    private:
        inline int GetUpgradePrice(int const treeSize) const
        {
            assert("INVALID tree size" && treeSize >= 0 && treeSize < MAX_TREE_SIZE);
            return TREE_LEVEL_UPGRADE_BASE_PRICE[treeSize] + m_myTreesCntBySize[treeSize + 1];
        }
        inline int GetSeedPrice() const { return m_myTreesCntBySize[0];}

        inline int GetMySun() const { return m_turnData.m_mySun; }
        inline int GetMyScore() const { return m_turnData.m_myScore; }
    };
}