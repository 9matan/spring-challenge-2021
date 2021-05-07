#pragma once

unsigned int InitializeRandom();
// exclusive max
unsigned int GetRandomUInt(unsigned int const minVal, unsigned int const maxVal);

template<typename TIter>
TIter GetRandomItem(TIter first, TIter last)
{
    size_t const rangeSize = static_cast<size_t>(last - first);
    return first + GetRandomUInt(0, static_cast<unsigned int>(rangeSize));
}

template<typename TEnum>
TEnum GetRandomEnumValue()
{
    return static_cast<TEnum>(GetRandomUInt(0, static_cast<unsigned int>(TEnum::Count)));
}