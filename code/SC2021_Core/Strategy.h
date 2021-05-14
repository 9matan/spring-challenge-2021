#pragma once

#include "CodingameUtility\VectorInPlace.h"

constexpr int MAX_TURNS_PER_DAY = 16;

namespace sc2021
{
    enum class ETurnStrategyType
    {
        Invalid = 0,
        CompleteLifeCycle,
        IncreaseIncome,
        UpgradeToLargeTree,
        SeedNewTree
    };

    struct STurnStrategy
    {
        using Condition = std::function<bool(STurnStrategy const&)>;

        ETurnStrategyType m_strategyType = ETurnStrategyType::Invalid;
        int m_iterationsCount = 1;
        Condition m_condition = nullptr;
        int m_minTreeSize = 0;
        int m_maxTreeSize = MAX_TREE_SIZE;

        inline void Invalidate() { m_strategyType = ETurnStrategyType::Invalid; }
        inline bool IsValid() const { return m_strategyType == ETurnStrategyType::Invalid; }
        inline bool CheckTreeSizeLimits(int const treeSize) const { return treeSize >= m_minTreeSize && treeSize <= m_maxTreeSize; }

        inline STurnStrategy& SetCondition(Condition condition) { m_condition = condition; return *this; }
        inline STurnStrategy& SetTreeSizeLimits(int const minSize, int const maxSize)
        {
            assert(minSize <= maxSize);
            assert(minSize >= 0);
            assert(maxSize <= MAX_TREE_SIZE);

            m_minTreeSize = minSize;
            m_maxTreeSize = maxSize;

            return *this;
        }
    };

    constexpr int INFINITY_ITERATIONS_COUNT = -1;

    #define INVALID_TURN_STRATEGY STurnStrategy()

    struct SDayStrategy
    {
        CVectorInPlace<STurnStrategy, MAX_TURNS_PER_DAY> m_turnStrategies;
        bool m_isValid = false;

        inline void Invalidate() { m_turnStrategies.clear(); m_isValid = false; }
        inline bool IsValid() const { return !m_turnStrategies.empty() || m_isValid; }
        inline void Validate() { m_isValid = true; }
    };

    #define INVALID_DAY_STRATEGY SDayStrategy()
}