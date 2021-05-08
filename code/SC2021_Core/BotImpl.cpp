#include "Core_PCH.h"
#include "BotImpl.h"

#include "CodingameUtility\Random.h"

using namespace std;

namespace sc2021
{
    namespace CBotImplPriv
    {
        SCommand CreateWaitCmd()
        {
            SCommand cmd;
            cmd.m_commandType = ECommandType::Wait;
            return cmd;
        }

        SCommand CreateCompleteCmd(int const cellIndex)
        {
            SCommand cmd;
            cmd.m_cellIndex = cellIndex;
            cmd.m_commandType = ECommandType::Complete;
            return cmd;
        }
    }

    STurnOutputData CBotImpl::FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData)
    {
        m_initData = initInData;
        int const seed = UpdateRandomSeed();
        cerr << "Bot seed: " << seed << "\n";

        return Update(turnInData);
    }
    
    STurnOutputData CBotImpl::Update(STurnInputData const& turnInData)
    {
        m_turnData = turnInData;

        STurnOutputData outputData;
        outputData.m_command = FindTurn();
        return outputData;
    }

    SCommand CBotImpl::FindTurn()
    {
        using namespace CBotImplPriv;

        int const sunPreCompleteCmd = 4;
        int const maxTreeSize = 3;

        if (m_turnData.m_mySun < sunPreCompleteCmd)
        {
            return CreateWaitCmd();
        }

        auto findCell = [this](int const cellIndex)
        {
            return std::find_if(m_initData.m_cells.begin(), m_initData.m_cells.end(), [cellIndex](auto const& cell)
                {
                    return cellIndex == cell.m_index;
                });
        };

        SInitCellInputData* curCell = nullptr;
        for (auto const& tree : m_turnData.m_trees)
        {
            if (tree.m_isMine && tree.m_size == maxTreeSize)
            {
                auto const cell = findCell(tree.m_cellIndex);
                if (curCell == nullptr || curCell->m_richness < cell->m_richness)
                {
                    curCell = cell;
                }
            }
        }

        if (!curCell || (curCell->m_richness <= 1 && m_turnData.m_nutriens == 0))
        {
            return CreateWaitCmd();
        }

        return CreateCompleteCmd(curCell->m_index);
    }
}