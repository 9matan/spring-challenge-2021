#pragma once

#include <algorithm>
#include <assert.h>

template<typename TComponent>
struct SVec2Base
{
    static const size_t ms_vecSize = 2;
    using ValueType = TComponent;

    template<typename TOtherComponent>
    explicit SVec2Base(SVec2Base<TOtherComponent> const other) 
        : m_x(static_cast<TComponent>(other[0]))
        , m_y(static_cast<TComponent>(other[1]))
    {}

    template<typename TOtherComponent>
    SVec2Base(TOtherComponent const x, TOtherComponent const y)
        : m_x(static_cast<TComponent>(x))
        , m_y(static_cast<TComponent>(y))
    {}

    template<typename TOtherComponent>
    SVec2Base(std::initializer_list<TOtherComponent> const& data)
    {
        assert(data.size() <= ms_vecSize);
        std::transform(data.begin(), data.end(), m_data,
            [](TOtherComponent const otherComp)
            {
                return static_cast<TComponent>(otherComp);
            });
    }

    SVec2Base() : m_x(0), m_y(0) {}
    SVec2Base(TComponent const x, TComponent const y) : m_x(x), m_y(y) {}
    SVec2Base(std::initializer_list<TComponent> const& data)
    { 
        assert(data.size() <= ms_vecSize); 
        std::copy(data.begin(), data.end(), m_data);
    }

    inline SVec2Base& operator=(SVec2Base const other) { m_data[0] = other.m_data[0]; m_data[1] = other.m_data[1]; return *this; }
    inline bool operator==(SVec2Base const other) const { return other[0] == m_data[0] && other[1] == m_data[1]; }
    inline bool operator!=(SVec2Base const other) const { return !(*this == other); }
    inline TComponent operator[](size_t const index) const { return m_data[index]; }
    inline TComponent& operator[](size_t const index) { return m_data[index]; }
    inline SVec2Base operator+(SVec2Base const other) const { return SVec2Base(m_data[0] + other.m_data[0], m_data[1] + other.m_data[1]); }
    inline SVec2Base operator-(SVec2Base const other) const { return SVec2Base(m_data[0] - other.m_data[0], m_data[1] - other.m_data[1]); }
    inline bool operator<(SVec2Base const other) const { if (m_data[0] == other[0]) { return m_data[1] < other[1]; } return m_data[0] < other[0]; }

private:
    union
    {
        struct
        {
            TComponent m_x;
            TComponent m_y;
        };
        TComponent m_data[ms_vecSize];
    };
};

using SVec2i = SVec2Base<int>;
using SVec2u = SVec2Base<unsigned int>;
using SVec2si = SVec2Base<short>;
using SVec2su = SVec2Base<unsigned short>;

// Set default for your project
using SVec2 = SVec2Base<char>;