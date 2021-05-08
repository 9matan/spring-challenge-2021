#include "Core_PCH.h"
#include "Bot.h"

#include "BotImpl.h"

using namespace std;

namespace sc2021
{
    CBot::CBot() : m_impl(make_unique<CBotImpl>()) {}
    CBot::~CBot() {}

    STurnOutputData CBot::FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData)
    {
        return m_impl->FirstUpdate(initInData, turnInData);
    }

    STurnOutputData CBot::Update(STurnInputData const& turnInData)
    {
        return m_impl->Update(turnInData);
    }
}