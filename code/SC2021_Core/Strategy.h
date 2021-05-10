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
        ETurnStrategyType m_strategyType = ETurnStrategyType::Invalid;
        // TODO: Replace m_repeat with counter
        bool m_repeat;

        inline void Invalidate() { m_strategyType = ETurnStrategyType::Invalid; }
        inline bool IsValid() const { return m_strategyType == ETurnStrategyType::Invalid; }
    };

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