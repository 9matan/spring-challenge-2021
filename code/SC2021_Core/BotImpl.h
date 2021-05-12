#pragma once

#include "InputData.h"
#include "Map.h"
#include "OutputData.h"
#include "ShadowManager.h"
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
        CMap m_map;
        CShadowManager m_shadowManager;
        int m_myTreesCntBySize[MAX_TREE_SIZE + 1];
        int m_oppTreesCntBySize[MAX_TREE_SIZE + 1];
    private:
        void InitMap(SInitInputData const& initData);

        void UpdateTrees();
        void UpdateShadowManager();

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
        void OnNewDay();

    private:
        inline int GetUpgradePrice(int const treeSize) const
        {
            assert("INVALID tree size" && treeSize >= 0 && treeSize < MAX_TREE_SIZE);
            return TREE_LEVEL_UPGRADE_BASE_PRICE[treeSize] + m_myTreesCntBySize[treeSize + 1];
        }
        inline int GetSeedPrice() const { return m_myTreesCntBySize[0];}

        inline int GetMySun() const { return m_turnData.m_mySun; }
        inline int GetMyScore() const { return m_turnData.m_myScore; }
        inline int GetDaysRemaining() const { return LAST_DAY_NUMBER - GetCurrentDay(); }
        inline int GetCurrentDay() const { return m_turnData.m_day; }
        inline int GetNutriens() const { return m_turnData.m_nutriens; }
        inline int GetTreesCount(int const treeSize) const
        {
            assert(treeSize <= MAX_TREE_SIZE && treeSize >= -1);
            if (treeSize == -1)
            {
                int res = 0;
                for (int i = 0; i <= MAX_TREE_SIZE; ++i)
                    res += GetTreesCount(i);
                return res;
            }
            return m_myTreesCntBySize[treeSize] + m_oppTreesCntBySize[treeSize];
        }
    };
}