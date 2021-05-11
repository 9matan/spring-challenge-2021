#pragma once

#include "CodingameUtility\VectorInPlace.h"

#include "CellEntity.h"
#include "GameConfig.h"

namespace sc2021
{
    class CMap
    {
    public:
        using CellPredicate = bool(*)(SCellEntity const&);
        using Cells = CVectorInPlace<SCellEntity*, MAX_CELLS_COUNT>;

    public:
        void GetCells(Cells& cells, CellPredicate predicate);

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