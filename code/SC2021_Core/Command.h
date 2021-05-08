#pragma once

namespace sc2021
{
	enum class ECommandType
	{
		Wait,
		Complete
	};

	struct SCommand
	{
		int m_cellIndex;
		ECommandType m_commandType;
	};
}