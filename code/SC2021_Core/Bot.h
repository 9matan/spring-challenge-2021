#pragma once

#include <memory>

#include "OutputData.h"

namespace sc2021
{
    class CBotImpl;
    struct SInitInputData;
    struct STurnInputData;

    class CBot
    {
    public:
        CBot();
        ~CBot();

        STurnOutputData FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData);
        STurnOutputData Update(STurnInputData const& turnInData);

    private:
        std::unique_ptr<CBotImpl> m_impl;
    };
}