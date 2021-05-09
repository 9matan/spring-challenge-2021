#pragma once

namespace sc2021
{
    struct STreeEntity
    {
        int m_size;
        bool m_isMine;
        bool m_isDormant;

        inline void Invalidate() { m_size = -1; }
        inline bool IsValid() const { return m_size != -1; }
    };
}