#include "Core_PCH.h"

#include "ShadowManager.h"

using namespace std;

namespace sc2021
{
    void CShadowManager::UpdateDarknessLevel(CMap& map, EDirection const currentShadowDirection, int const daysToSimulate)
    {
        assert(daysToSimulate <= SM_MAX_DAYS_TO_SIMULATE);

        for (int day = 0; day <= daysToSimulate; ++day)
        {
            for (int cellIndex = 0; cellIndex < MAX_CELLS_COUNT; ++cellIndex)
            {
                for (int treeLevel = 0; treeLevel < MAX_TREE_SIZE; ++treeLevel)
                {
                    m_darknessLevel[day][cellIndex][treeLevel] = 0.0f;
                }
            }
        }

        CMap::CellEntries cellEntries;
        EDirection sunDirection = GetInverseDirection(currentShadowDirection);
        for (int day = 1; day <= daysToSimulate; ++day)
        {
            sunDirection = GetNextDirection(sunDirection);
            for (auto const& cell : map)
            {
                for (int treeLevel = 0; treeLevel < MAX_TREE_SIZE; ++treeLevel)
                {
                    map.GetCellsInDirection(cellEntries, cell.m_index, sunDirection, MAX_TREE_SIZE);
                    float const darkness = CalculateCellDarknessLevel(treeLevel, day, cellEntries);

                    if (darkness > 1.0f || darkness < 0.0f)
                    {
                        cerr << "[ERROR] Invalid darkness level: " << darkness
                            << " cell index: " << cell.m_index
                            << " cell neigh: " << cellEntries.front().m_cell->m_index
                            << "\n";
                    }

                    m_darknessLevel[day][cell.m_index][treeLevel] = max(min(darkness, 1.0f), 0.0f);;
                }
            }
        }

        m_numberOfSimulatedDays = daysToSimulate;
    }

    float CShadowManager::GetDarknessLevelInRange_Avrg(int const cellIndex, int const startDay, int const endDay, int const treeLevel)
    {
        auto avrgFunction = [](int startDay, int endDay, int curDay)
        {
            return 1.0f / (float)(endDay - startDay + 1);
        };
        return GetDarknessLevelInRange(cellIndex, startDay, endDay, avrgFunction, treeLevel);
    }

    float CShadowManager::GetDarknessLevelInRange(int const cellIndex, int const startDay, int const endDay, DayCoefCalculator dayCoefCalculator, int const treeLevel)
    {
        if (startDay > m_numberOfSimulatedDays)
        {
            return 0.0f;
        }
        int const validateEndDay = (endDay == -1)
            ? m_numberOfSimulatedDays
            : min(endDay, m_numberOfSimulatedDays);

        assert(startDay <= validateEndDay);
        assert(startDay >= 0);
        assert(validateEndDay > 0);
        if (startDay < 1)
        {
            cerr << "[ERROR] Invalid start day: " << startDay
                << " simulation starts from 1st day"
                << "\n";
        }

        float darkness = 0.0f;
        for (int day = max(startDay, 1); day <= validateEndDay; ++day)
        {
            float const currentDayCoef = (dayCoefCalculator) ? dayCoefCalculator(startDay, validateEndDay, day) : 1.0f;
            darkness += m_darknessLevel[day][cellIndex][treeLevel] * currentDayCoef;
        }

        return darkness;
    }

    float CShadowManager::CalculateCellDarknessLevel(int const treeLevel, int const day, CMap::CellEntries const& entries)
    {
        int curLevelToOutshine = -1;
        int curTreeLevel = -1;

        for (auto const& cellEntry : entries)
        {
            if (!cellEntry.m_cell->HasTree()) continue;
            auto const cell = cellEntry.m_cell;
            
            int const levelDiff = max(0, treeLevel - cell->m_tree.m_size);
            int const distanceToShadow = max(0, cellEntry.m_distance - cell->m_tree.m_size);
            int const levelToOutshine = max(levelDiff, distanceToShadow);

            if (curLevelToOutshine == -1
                || curLevelToOutshine > levelToOutshine
                || (curLevelToOutshine == levelToOutshine && curTreeLevel > cell->m_tree.m_size))
            {
                curLevelToOutshine = levelToOutshine;
                curTreeLevel = cell->m_tree.m_size;
            }
        }

        if (curLevelToOutshine != -1)
        {
            float darkness = 0.0f;
            if (curLevelToOutshine > 0)
            {
                assert(curTreeLevel != -1);

                float const majorStep = 1.0f / (float)MAX_TREE_SIZE;
                float const minorStep = majorStep / ((float)MAX_TREE_SIZE * 2.0f);
                darkness = (curLevelToOutshine <= day)
                    ? (MAX_TREE_SIZE - curLevelToOutshine) * majorStep + (MAX_TREE_SIZE - curTreeLevel) * minorStep
                    : 0.0f;
            }
            else
            {
                darkness = 1.0f;
            }

            return darkness;
        }

        return 0.0f;
    }
}