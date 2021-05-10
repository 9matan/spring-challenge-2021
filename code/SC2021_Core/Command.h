#pragma once

namespace sc2021
{
	enum class ECommandType
	{
        Invalid = 0,
		Wait,
		Complete,
        Grow,
        Seed
	};

	struct SCommand
	{
		int m_cellIndex;
        int m_seedCellIndex;
		ECommandType m_commandType = ECommandType::Invalid;

        inline void Invalidate() { m_commandType = ECommandType::Invalid; }
        inline bool IsValid() const { return m_commandType != ECommandType::Invalid; }
	};

    #define INVALID_COMMAND SCommand()
}