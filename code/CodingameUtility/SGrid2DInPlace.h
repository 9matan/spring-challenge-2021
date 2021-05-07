#pragma once

#include <assert.h>

#include "Vec2.h"
#include "VectorInPlace.h"

template <typename TElem, size_t TMaxWidth, size_t TMaxHeight>
struct SGrid2DInPlace
{
public:
    SGrid2DInPlace(size_t const width, size_t const height)
        : m_width(width)
        , m_height(height)
    {
        assert(width <= TMaxWidth && height <= TMaxHeight);
        m_elements.resize(width * height);
    }

    // x, y
    inline TElem const& GetElement(SVec2 const pos) const { return m_elements[GetIndex(pos)]; }
    inline TElem GetElementCopy(SVec2 const pos) const { return m_elements[GetIndex(pos)]; }
    inline TElem& ModifyElement(SVec2 const pos) { return m_elements[GetIndex(pos)]; }
    inline void SetElement(SVec2 const pos, TElem const& elem) { m_elements[GetIndex(pos)] = elem; }

    inline size_t GetWidth() const { return m_width; }
    inline size_t GetHeight() const { return m_height; }
    inline size_t GetSize() const { return m_elements.size(); }

    inline bool IsValidPos(SVec2 const pos) const { return pos[0] >= 0 && pos[0] < m_width && pos[1] >= 0 && pos[1] < m_height; }
    inline size_t GetIndex(SVec2 const pos) const { return pos[0] + pos[1] * m_width; }

    inline TElem* begin() { return m_elements.begin(); }
    inline TElem* end() { return m_elements.end(); }
    inline TElem const* begin() const { return m_elements.begin(); }
    inline TElem const* end() const { return m_elements.end(); }

private:
    CVectorInPlace<TElem, TMaxWidth * TMaxHeight> m_elements;
    size_t m_width;
    size_t m_height;
};