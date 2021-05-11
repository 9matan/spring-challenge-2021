#include "Core_PCH.h"

#include "Map.h"

namespace sc2021
{
    void CMap::GetCells(Cells& cells, CellPredicate predicate)
    {
        cells.clear();

        for (auto& cell : m_cells)
        {
            if (predicate(cell))
            {
                cells.push_back(&cell);
            }
        }
    }

    void CMap::GetCellsInRadius(Cells& cells, int const cellIndex, int const radius)
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

            cells.push_back(&cell);
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
}