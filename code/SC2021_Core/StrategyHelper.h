#pragma once

#include "Strategy.h"

#include <string>

namespace sc2021
{
    char const* ToString(STurnStrategy const& turnStrategy);

    STurnStrategy CreateCompleteLifeCycleTS(bool const repeat = false);
    STurnStrategy CreateIncreaseIncomeTS(bool const repeat = false);
    STurnStrategy CreateUpgradeToLargeTreeTS(bool const repeat = false);
    STurnStrategy CreateSeedNewTreeTS(bool const repeat = false);
}