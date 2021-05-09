#pragma once

namespace sc2021
{
	enum class ECommandType
	{
        Invalid = 0,
		Wait,
		Complete,
        Grow
	};

	struct SCommand
	{
		int m_cellIndex;
		ECommandType m_commandType = ECommandType::Invalid;

        inline void Invalidate() { m_commandType = ECommandType::Invalid; }
        inline bool IsValid() const { return m_commandType != ECommandType::Invalid; }
	};

    inline SCommand CreateWaitCmd()
    {
        SCommand cmd;
        cmd.m_commandType = ECommandType::Wait;
        return cmd;
    }

    inline SCommand CreateCompleteCmd(int const cellIndex)
    {
        SCommand cmd;
        cmd.m_cellIndex = cellIndex;
        cmd.m_commandType = ECommandType::Complete;
        return cmd;
    }

    inline SCommand CreateGrowCmd(int const cellIndex)
    {
        SCommand cmd;
        cmd.m_cellIndex = cellIndex;
        cmd.m_commandType = ECommandType::Grow;
        return cmd;
    }
}