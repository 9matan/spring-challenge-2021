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
}