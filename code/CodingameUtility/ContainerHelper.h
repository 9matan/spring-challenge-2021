#pragma once

#include <vector>

template<typename TElem>
void EraseSwapWithLast(std::vector<TElem>& v, TElem const elem)
{
    auto iter = std::find(v.begin(), v.end(), elem);
    if (iter == v.end())
    {
        return;
    }

    std::swap(*iter, v.back());
    v.pop_back();
}