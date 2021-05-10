#pragma once

#include <string>
#include <vector>

#include "CodingameUtility\VectorInPlace.h"

#include "Direction.h"
#include "GameConfig.h"

namespace sc2021
{
    struct SInitCellInputData
    {
        int m_index;
        int m_richness;
        int m_neighs[DIRECTIONS_COUNT];
    };

    struct SInitInputData
    {
        CVectorInPlace<SInitCellInputData, MAX_CELLS_COUNT> m_cells;
    };

    struct STreeInputData
    {
        int m_cellIndex;
        int m_size;
        bool m_isMine;
        bool m_isDormant;
    };

    struct STurnInputData
    {
        int m_day;
        int m_nutriens;

        int m_mySun;
        int m_myScore;

        int m_oppSun;
        int m_oppScore;
        bool m_oppWaiting;

        CVectorInPlace<STreeInputData, MAX_CELLS_COUNT> m_trees;
        std::vector<std::string> m_possibleActions;
    };
}