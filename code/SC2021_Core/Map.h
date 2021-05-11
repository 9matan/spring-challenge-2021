#pragma once

#include "CodingameUtility\VectorInPlace.h"

#include "CellEntity.h"
#include "GameConfig.h"

namespace sc2021
{
    class CMap
    {
    public:
        struct SCellEntry
        {
            SCellEntry() : SCellEntry(nullptr) {}
            SCellEntry(SCellEntity* cell, int distance = -1)
                : m_cell(cell)
                , m_distance(distance)
            {
            }

            SCellEntity* m_cell;
            int m_distance;
        };

    public:
        using CellPredicate = bool(*)(SCellEntity const&);
        using Cells = CVectorInPlace<SCellEntity*, MAX_CELLS_COUNT>;
        using CellEntries = CVectorInPlace<SCellEntry, MAX_CELLS_COUNT>;

    public:
        void GetCells(Cells& cells, CellPredicate predicate);
        void GetCellsInRadius(CellEntries& cells, int const cellIndex, int const radius);
        void GetCellsInDirection(CellEntries& cells, int const cellIndex, EDirection const direction, int const maxDistance);

    public:
        inline SCellEntity* begin() { return std::begin(m_cells); }
        inline SCellEntity* end() { return std::end(m_cells); }
        inline SCellEntity& operator[](size_t const index) { return m_cells[index]; }

        inline SCellEntity const* begin() const { return std::begin(m_cells); }
        inline SCellEntity const* end() const { return std::end(m_cells); }
        inline SCellEntity const& operator[](size_t const index) const { return m_cells[index]; }
    private:
        SCellEntity m_cells[MAX_CELLS_COUNT];
    };
}