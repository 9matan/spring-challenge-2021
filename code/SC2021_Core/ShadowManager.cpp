#include "Core_PCH.h"

#include "ShadowManager.h"

namespace sc2021
{
    void CShadowManager::UpdateDarknessLevel(CMap& map, EDirection currentShadowDirection)
    {
        for (int cellIndex = 0; cellIndex < MAX_CELLS_COUNT; ++cellIndex)
        {
            for (int treeLevel = 0; treeLevel < MAX_TREE_SIZE; ++treeLevel)
            {
                m_darknessLevel[cellIndex][treeLevel] = 0.0f;
            }
        }

        CMap::CellEntries cellEntries;
        for (int day = 1; day <= m_config.m_numberOfDaysToSimulate; ++day)
        {
            currentShadowDirection = GetNextDirection(currentShadowDirection);
            for (auto const& cell : map)
            {
                map.GetCellsInDirection(cellEntries, cell.m_index, GetInverseDirection(currentShadowDirection), MAX_TREE_SIZE);
                UpdateCellDarknessLevel(cell.m_index, cellEntries, day);
            }
        }

        for (int cellIndex = 0; cellIndex < MAX_CELLS_COUNT; ++cellIndex)
        {
            for (int treeLevel = 0; treeLevel < MAX_TREE_SIZE; ++treeLevel)
            {
                m_darknessLevel[cellIndex][treeLevel] /= (float)m_config.m_numberOfDaysToSimulate;
            }
        }
    }

    void CShadowManager::UpdateCellDarknessLevel(int const cellIndex, CMap::CellEntries const& entries, int const day)
    {
        for (int treeLevel = 0; treeLevel < MAX_TREE_SIZE; ++treeLevel)
        {
            int curLevelToOutshine = -1;
            int curTreeLevel = -1;

            for (auto const& cellEntry : entries)
            {
                if (!cellEntry.m_cell->HasTree()) continue;
                auto const cell = cellEntry.m_cell;
            
                int const levelDiff = std::max(0, treeLevel - cell->m_tree.m_size);
                int const distanceToShadow = std::max(0, cellEntry.m_distance - cell->m_tree.m_size);
                int const levelToOutshine = std::max(levelDiff, distanceToShadow);

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

                    if (darkness > 1.0f || darkness < 0.0f)
                    {
                        std::cerr << "[ERROR] Invalid darkness level: " << darkness
                            << " cell index: " << cellIndex
                            << " cell neigh: " << entries.front().m_cell->m_index
                            << "\n";
                    }
                }
                else
                {
                    darkness = 1.0f;
                }

                m_darknessLevel[cellIndex][treeLevel] += std::max(std::min(darkness, 1.0f), 0.0f);
            }
        }
    }
}