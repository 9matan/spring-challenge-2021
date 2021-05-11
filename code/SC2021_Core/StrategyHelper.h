#pragma once

#include "Strategy.h"

#include <string>

namespace sc2021
{
    char const* ToString(STurnStrategy const& turnStrategy);

    STurnStrategy CreateCompleteLifeCycleTS(int const iterationsCount = 1);
    STurnStrategy CreateIncreaseIncomeTS(int const iterationsCount = 1);
    STurnStrategy CreateUpgradeToLargeTreeTS(int const iterationsCount = 1);
    STurnStrategy CreateSeedNewTreeTS(int const iterationsCount = 1);
}