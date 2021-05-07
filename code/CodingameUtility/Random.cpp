#include "CodingameUtility_PCH.h"

#include <assert.h>
#include <random>
#include <time.h>

unsigned int InitializeRandom()
{
    unsigned int const seed = (unsigned int)time(nullptr);
    srand(seed);
    return seed;
}

unsigned int GetRandomUInt(unsigned int const minVal, unsigned int const maxVal)
{
    assert(minVal < maxVal);
    unsigned int const range = maxVal - minVal;
    return rand() % range + minVal;
}