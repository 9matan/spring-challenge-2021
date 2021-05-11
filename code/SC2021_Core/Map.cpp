#include "Core_PCH.h"

#include "Map.h"

namespace sc2021
{
    void CMap::GetCells(Cells& cells, CellPredicate predicate)
    {
        cells.clear();
        cells.reserve(MAX_CELLS_COUNT);

        for (auto& cell : m_cells)
        {
            if (predicate(cell))
            {
                cells.push_back(&cell);
            }
        }
    }

    void CMap::GetCellsInRadius(CellEntries& cells, int const cellIndex, int const radius)
    {
        assert(cellIndex >= 0 && cellIndex < MAX_CELLS_COUNT);
        cells.clear();
        cells.reserve(MAX_CELLS_COUNT);

        bool added[MAX_CELLS_COUNT];
        memset(added, 0, sizeof(added));
        added[cellIndex] = true;

        // index, distance
        std::queue<std::pair<int, int>> q;
        q.push(std::make_pair(cellIndex, 0));

        while (!q.empty())
        {
            auto const cur = q.front();
            int const index = cur.first;
            int const distance = cur.second;
            auto& cell = m_cells[index];
            q.pop();

            cells.push_back(SCellEntry(&cell, distance));
            if (distance < radius)
            {
                for (auto& neigh : cell.m_neigh)
                {
                    if (!neigh) continue;
                    if (added[neigh->m_index]) continue;
                    
                    added[neigh->m_index] = true;
                    q.push(std::make_pair(neigh->m_index, distance + 1));
                }
            }
        }
    }

    void CMap::GetCellsInDirection(CellEntries& cells, int const cellIndex, EDirection const direction, int const maxDistance)
    {
        assert(cellIndex >= 0 && cellIndex < MAX_CELLS_COUNT);
        cells.clear();
        cells.reserve(MAX_CELLS_COUNT);

        int const directionIndex = (int)direction;

        SCellEntity* curCell = m_cells + cellIndex;
        int distance = 0;
        while (true)
        {
            ++distance;
            if (distance > maxDistance) break;
            SCellEntity* nxtCell = curCell->m_neigh[directionIndex];
            if (!nxtCell) break;
            curCell = nxtCell;

            cells.push_back(SCellEntry(nxtCell, distance));
        }
    }
}