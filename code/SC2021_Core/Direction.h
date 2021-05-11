#pragma once

namespace sc2021
{
    enum class EDirection
    {
        Right = 0,
        UpRight,
        UpLeft,
        Left,
        DownLeft,
        DownRight,
        Count
    };

    constexpr int DIRECTIONS_COUNT = (int)EDirection::Count;

    inline EDirection GetNextDirection(EDirection const direction) { return (EDirection)(((int)direction + 1) % DIRECTIONS_COUNT); }    
    inline EDirection GetCurrentShadowDirection(int const dayIndex) { return (EDirection)(dayIndex % DIRECTIONS_COUNT); }
    inline EDirection GetInverseDirection(EDirection const direction) { return (EDirection)(((int)direction + DIRECTIONS_COUNT / 2) % DIRECTIONS_COUNT); }
}