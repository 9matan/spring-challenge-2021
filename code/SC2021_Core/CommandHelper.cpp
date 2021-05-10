#include "Core_PCH.h"
#include "CommandHelper.h"

#include "Command.h"

namespace sc2021
{
    char const* ToString(SCommand const& cmd)
    {
        static char buff[256];
        switch (cmd.m_commandType)
        {
        case ECommandType::Invalid:
            sprintf(buff, "Invalid");
            break;
        case ECommandType::Wait:
            sprintf(buff, "WAIT");
            break;
        case ECommandType::Complete:
            sprintf(buff, "COMPLETE %i", cmd.m_cellIndex);
            break;
        case ECommandType::Grow:
            sprintf(buff, "GROW %i", cmd.m_cellIndex);
            break;
        case ECommandType::Seed:
            sprintf(buff, "SEED %i %i", cmd.m_cellIndex, cmd.m_seedCellIndex);
            break;
        default:
            assert(false);
        }

        return buff;
    }

    SCommand CreateWaitCmd()
    {
        SCommand cmd;
        cmd.m_commandType = ECommandType::Wait;
        return cmd;
    }

    SCommand CreateCompleteCmd(int const cellIndex)
    {
        SCommand cmd;
        cmd.m_cellIndex = cellIndex;
        cmd.m_commandType = ECommandType::Complete;
        return cmd;
    }

    SCommand CreateGrowCmd(int const cellIndex)
    {
        SCommand cmd;
        cmd.m_cellIndex = cellIndex;
        cmd.m_commandType = ECommandType::Grow;
        return cmd;
    }

    SCommand CreateSeedCmd(int const cellIndex, int const seedCellIndex)
    {
        SCommand cmd;
        cmd.m_cellIndex = cellIndex;
        cmd.m_seedCellIndex = seedCellIndex;
        cmd.m_commandType = ECommandType::Seed;
        return cmd;
    }
}