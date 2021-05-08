#include "Core_PCH.h"
#include "CommandHelper.h"

#include "Command.h"

namespace sc2021
{
    std::string ToString(SCommand const& cmd)
    {
        char buff[256];
        switch (cmd.m_commandType)
        {
        case ECommandType::Wait:
            sprintf(buff, "WAIT");
            break;
        case ECommandType::Complete:
            sprintf(buff, "COMPLETE %i", cmd.m_cellIndex);
            break;
        default:
            assert(false);
        }

        return buff;
    }
}