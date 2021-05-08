#include "CodingameUtility\CommonDefs.h"
#include "SC2021_Core\GameConfig.h"

#include <iostream>

#include "CodingameUtility\Profiler.h"

#include "SC2021_Core\Bot.h"
#include "SC2021_Core\CommandHelper.h"
#include "SC2021_Core\InputData.h"
#include "SC2021_Core\OutputData.h"

using namespace sc2021;
using namespace std;

void ReadInitInputData(SInitInputData& data)
{
    int numberOfCells;
    cin >> numberOfCells; cin.ignore();
    cerr << numberOfCells << "\n";
    data.m_cells.resize(numberOfCells);

    for (auto& cell : data.m_cells)
    {
        cin >> cell.m_index
            >> cell.m_richness;
        cerr << cell.m_index << " "
            << cell.m_richness;

        cell.m_neighs.resize(MAX_NEIGHS_COUNT);
        for (auto& neigh : cell.m_neighs)
        {
            cin >> neigh;
            cerr << " " << neigh;
        }
        cin.ignore();
        cerr << "\n";
    }
}

void ReadTurnInputData(STurnInputData& data)
{
    cin >> data.m_day; cin.ignore();
    cin >> data.m_nutriens; cin.ignore();
    cin >> data.m_mySun
        >> data.m_myScore; cin.ignore();
    cin >> data.m_oppSun
        >> data.m_oppScore
        >> data.m_oppWaiting; cin.ignore();

    int numberOfTrees;
    cin >> numberOfTrees; cin.ignore();
    data.m_trees.resize(numberOfTrees);
    for (auto& tree : data.m_trees)
    {
        cin >> tree.m_cellIndex;
        cin >> tree.m_size;
        cin >> tree.m_isMine;
        cin >> tree.m_isDormant; cin.ignore();
    }

    int numberOfPossibleActions;
    cin >> numberOfPossibleActions; cin.ignore();
    data.m_possibleActions.resize(numberOfPossibleActions);
    for (auto& action : data.m_possibleActions)
    {
        getline(cin, action);
    }
}

void WriteTurnOutputData(STurnOutputData const& data)
{
    cout << ToString(data.m_command) << "\n";
    fflush(stderr);
    fflush(stdout);
}

int main()
{
    SInitInputData initInputData;
    STurnInputData turnInputData;
    STurnOutputData turnOutputData;
    CBot bot;

    ReadInitInputData(initInputData);
    ReadTurnInputData(turnInputData);
    {
        PROFILE_TIME("First update");
        turnOutputData = bot.FirstUpdate(initInputData, turnInputData);
    }
    WriteTurnOutputData(turnOutputData);

    while (true)
    {
        ReadTurnInputData(turnInputData);
        {
            PROFILE_TIME("Update");
            turnOutputData = bot.Update(turnInputData);
        }
        WriteTurnOutputData(turnOutputData);
    }

    return 0;
}