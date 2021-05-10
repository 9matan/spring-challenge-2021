#pragma once

#include "Direction.h"
#include "TreeEntity.h"

namespace sc2021
{
    struct STreeEntity;
    struct SCellEntity
    {
        STreeEntity m_tree;
        int m_index;
        int m_richness;
        SCellEntity* m_neigh[DIRECTIONS_COUNT] = { 0 };

        inline bool HasMyTree_Dormant(bool const isDormant) const { return HasMyTree() && m_tree.m_isDormant == isDormant; }
        inline bool HasMyTree() const { return m_tree.IsValid() && m_tree.m_isMine; }
        inline bool HasTree() const { return m_tree.IsValid(); }
    };
}