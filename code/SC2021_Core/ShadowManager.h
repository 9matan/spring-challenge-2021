#pragma once

#include "Direction.h"
#include "GameConfig.h"
#include "Map.h"

namespace sc2021
{
    constexpr int SM_MAX_DAYS_TO_SIMULATE = 6;

    class CShadowManager
    {
    public:
        using DayCoefCalculator = std::function<float(int startDay, int endDay, int curDay)>;

    public:
        void UpdateDarknessLevel(CMap& map, EDirection const currentShadowDirection, int const daysToSimulate);

        float GetDarknessLevelInRange_Avrg(int const cellIndex, int const startDay, int const endDay, int const treeLevel = 0);
        float GetDarknessLevelInRange(int const cellIndex, int const startDay, int const endDay, DayCoefCalculator dayCoefCalculator, int const treeLevel = 0);

    private:
        int m_numberOfSimulatedDays;
        float m_darknessLevel[SM_MAX_DAYS_TO_SIMULATE + 1][MAX_CELLS_COUNT][MAX_TREE_SIZE + 1];

    private:
        float CalculateCellDarknessLevel(int const treeLevel, int const day, CMap::CellEntries const& entries);
    };
}