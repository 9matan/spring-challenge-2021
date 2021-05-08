#pragma once

#include "InputData.h"
#include "OutputData.h"

namespace sc2021
{
    class CBotImpl
    {
    public:
        STurnOutputData FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData);
        STurnOutputData Update(STurnInputData const& turnInData);
    private:
        SInitInputData m_initData;
        STurnInputData m_turnData;
    private:
        SCommand FindTurn();
    };
}