#pragma once

#include <algorithm>
#include <vector>

#include "VectorInPlace.h"

class CRanges
{
public:
    CRanges(std::initializer_list<int> const& rangePoints)
    {
        assert(std::is_sorted(rangePoints.begin(), rangePoints.end()));
        m_rangesPoints.assign(rangePoints.begin(), rangePoints.end());
    }

    inline size_t GetRangeIndex(int const value) const { return std::upper_bound(m_rangesPoints.begin(), m_rangesPoints.end(), value) - m_rangesPoints.begin(); }
private:
    std::vector<int> m_rangesPoints;
};

template<size_t RangesCount, typename TFunc>
class CCompositeRangeFunction
{
public:
    using InRangesPoints = CVectorInPlace<int, RangesCount - 1>;
    using InFunctions = CVectorInPlace<TFunc, RangesCount>;

public:
    CCompositeRangeFunction(InRangesPoints const& ranges, InFunctions const& functions)
    {
        // TODO: make static assert
        assert(InRangesPoints::MAX_SIZE + 1 == InFunctions::MAX_SIZE);
        assert(ranges.is_full());
        assert(functions.is_full());
        assert(std::is_sorted(ranges.begin(), ranges.end()));

        std::copy(ranges.begin(), ranges.end(), m_ranges);
        std::copy(functions.begin(), functions.end(), m_functions);
    }

    size_t GetRangeIndex(int const value) const { return std::upper_bound(m_ranges, m_ranges + InRangesPoints::MAX_SIZE, value) - m_ranges;  }
    TFunc& GetFunctionByValue(int const value) { return m_functions[GetRangeIndex(value)]; }
private:
    int m_ranges[InRangesPoints::MAX_SIZE];
    TFunc m_functions[RangesCount];
};