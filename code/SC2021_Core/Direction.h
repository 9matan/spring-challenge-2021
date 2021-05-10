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
}