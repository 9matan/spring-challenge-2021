#pragma once

#include "CellEntity.h"
#include "InputData.h"
#include "OutputData.h"
#include "Strategy.h"

namespace sc2021
{
    class CBotImpl
    {
    public:
        STurnOutputData FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData);
        STurnOutputData Update(STurnInputData const& turnInData);
    private:
        STurnInputData m_turnData;
        CVectorInPlace<SDayStrategy, MAX_DAYS_COUNT> m_dayStrategies;

        SDayStrategy m_currentDayStrategy;
        CVectorInPlace<SCellEntity, MAX_CELLS_COUNT> m_cells;
        int m_myTreesCntBySize[MAX_TREE_SIZE + 1];
    private:
        void InitCells(SInitInputData const& initData);

        void UpdateTrees(STurnInputData const& turnInData);

        SCommand FindTurn();
        SCommand FindTurnByStrategy(STurnStrategy const turnStrategy);

        // turn strategy
        SCommand FindTurn_CompleteLifeCycle();
        SCommand FindTurn_IncreaseIncome();
        SCommand FindTurn_UpgradeToLargeTree();

        void OnNewDay(STurnInputData const& newData);

    private:
        inline int GetUpgradePrice(int const treeSize) const
        { 
            assert("INVALID tree size" && treeSize > 0 && treeSize < MAX_TREE_SIZE);
            return TREE_LEVEL_UPGRADE_BASE_PRICE[treeSize] + m_myTreesCntBySize[treeSize + 1];
        }

        inline int GetMySun() const { return m_turnData.m_mySun; }
        inline int GetMyScore() const { return m_turnData.m_myScore; }
    };
}