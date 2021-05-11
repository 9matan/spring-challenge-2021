#include "Core_PCH.h"

#include "StrategyHelper.h"

namespace sc2021
{
    char const* ToString(STurnStrategy const& turnStrategy)
    {
        static char buff[256];
        int curPos = 0;

        if (!turnStrategy.IsValid())
        {
            switch (turnStrategy.m_strategyType)
            {
            case ETurnStrategyType::CompleteLifeCycle:
                curPos = sprintf(buff, "CompleteLifeCycle");
                break;
            case ETurnStrategyType::IncreaseIncome:
                curPos = sprintf(buff, "IncreaseIncome");
                break;
            case ETurnStrategyType::UpgradeToLargeTree:
                curPos = sprintf(buff, "UpgradeToLargeTree");
                break;
            case ETurnStrategyType::SeedNewTree:
                curPos = sprintf(buff, "SeedNewTree");
                break;
            default:
                assert(!"Unhandled ETurnStrategyType");
            }

            if (turnStrategy.m_iterationsCount == INFINITY_ITERATIONS_COUNT)
            {
                curPos = sprintf(buff + curPos, " (endless)");
            }
            else
            {
                curPos = sprintf(buff + curPos, " (x%i)", turnStrategy.m_iterationsCount);
            }
        }
        else
        {
            sprintf(buff, "Invalid");
        }        

        return buff;
    }

    STurnStrategy CreateCompleteLifeCycleTS(int const iterationsCount)
    {
        STurnStrategy strategy;
        strategy.m_strategyType = ETurnStrategyType::CompleteLifeCycle;
        strategy.m_iterationsCount = iterationsCount;
        return strategy;
    }

    STurnStrategy CreateIncreaseIncomeTS(int const iterationsCount)
    {
        STurnStrategy strategy;
        strategy.m_strategyType = ETurnStrategyType::IncreaseIncome;
        strategy.m_iterationsCount = iterationsCount;
        return strategy;
    }

    STurnStrategy CreateUpgradeToLargeTreeTS(int const iterationsCount)
    {
        STurnStrategy strategy;
        strategy.m_strategyType = ETurnStrategyType::UpgradeToLargeTree;
        strategy.m_iterationsCount = iterationsCount;
        return strategy;
    }

    STurnStrategy CreateSeedNewTreeTS(int const iterationsCount)
    {
        STurnStrategy strategy;
        strategy.m_strategyType = ETurnStrategyType::SeedNewTree;
        strategy.m_iterationsCount = iterationsCount;
        return strategy;
    }
}