#pragma once

#include "Direction.h"
#include "GameConfig.h"
#include "Map.h"

namespace sc2021
{
    struct SShadowManagerConfig
    {
        int m_numberOfDaysToSimulate;
    };

    class CShadowManager
    {
    public:
        void UpdateDarknessLevel(CMap& map, EDirection currentShadowDirection);

        SShadowManagerConfig& ModifyConfig() { return m_config; }

        float GetDarknessLevel(int const cellIndex, int const treeLevel = 0) const { return m_darknessLevel[cellIndex][treeLevel]; }

    private:
        SShadowManagerConfig m_config;
        float m_darknessLevel[MAX_CELLS_COUNT][MAX_TREE_SIZE + 1];

    private:
        void UpdateCellDarknessLevel(int const cellIndex, CMap::CellEntries const& entries, int const day);
    };
}