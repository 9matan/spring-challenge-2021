#include "CodingameUtility_PCH.h"
#include "Profiler.h"

#include <iostream>

using namespace std;

namespace 
{
    static thread_local int gNestedLevel = 0;
}

CTimeProfiler::CTimeProfiler(char const* const name)
    : m_name(name)
    , m_startTime(chrono::high_resolution_clock::now())
{
    ++gNestedLevel;
}

CTimeProfiler::~CTimeProfiler()
{
    auto const end = chrono::high_resolution_clock::now();
    double const executionTime = (double)chrono::duration_cast<chrono::nanoseconds>(end - m_startTime).count();
    string const tabs(gNestedLevel, '-');
    cerr.precision(3);
    cerr << "[Profiler] " << tabs << m_name << ": " << executionTime / 1000000.0 << fixed << " ms" << endl;
    --gNestedLevel;
}
