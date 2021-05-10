#pragma once

#include "Command.h"

#include <string>

namespace sc2021
{
    char const* ToString(SCommand const& cmd);

    SCommand CreateWaitCmd();
    SCommand CreateCompleteCmd(int const cellIndex);
    SCommand CreateGrowCmd(int const cellIndex);
    SCommand CreateSeedCmd(int const cellIndex, int const seedCellIndex);
}