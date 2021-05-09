#pragma once

#include "CodingameUtility\VectorInPlace.h"

constexpr int MAX_TURNS_PER_DAY = 16;

namespace sc2021
{
    enum class EDayStrategyType
    {
        Default = 0,
        Predefined
    };

    enum class ETurnStrategyType
    {
        CompleteLifeCycle = 0,
        IncreaseIncome = 1,
        UpgradeToLargeTree = 2
    };

    struct STurnStrategy
    {
        ETurnStrategyType m_strategyType;
        bool m_repeat;
    };

    struct SDayStrategy
    {
        EDayStrategyType m_strategyType;
        CVectorInPlace<STurnStrategy, MAX_TURNS_PER_DAY> m_turnStrategies;
    };

    inline STurnStrategy CreateCompleteLifeCycleTS(bool const repeat = false)
    {
        STurnStrategy strategy;
        strategy.m_strategyType = ETurnStrategyType::CompleteLifeCycle;
        strategy.m_repeat = repeat;
        return strategy;
    }

    inline STurnStrategy CreateIncreaseIncomeTS(bool const repeat = false)
    {
        STurnStrategy strategy;
        strategy.m_strategyType = ETurnStrategyType::IncreaseIncome;
        strategy.m_repeat = repeat;
        return strategy;
    }

    inline STurnStrategy CreateUpgradeToLargeTreeTS(bool const repeat = false)
    {
        STurnStrategy strategy;
        strategy.m_strategyType = ETurnStrategyType::UpgradeToLargeTree;
        strategy.m_repeat = repeat;
        return strategy;
    }
}