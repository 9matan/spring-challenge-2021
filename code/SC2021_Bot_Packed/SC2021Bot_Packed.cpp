
#if defined(__GNUC__) || defined(__GNUG__)
#pragma GCC optimize("O3")
#pragma GCC optimize("inline")
#pragma GCC optimize("omit-frame-pointer")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("Ofast")
#endif 
#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

constexpr int MAX_NEIGHS_COUNT = 6;constexpr int MAX_CELLS_COUNT = 37;

#include <chrono>
#include <ctime>
#include <string>
class CTimeProfiler{public:CTimeProfiler(char const* const name);~CTimeProfiler();private:std::string m_name;decltype(std::chrono::high_resolution_clock::now()) m_startTime;};
#ifndef DISABLE_PROFILE
#define PROFILE_TIME(name) CTimeProfiler timeProfiler(name);
#else
#define PROFILE_TIME(name)
#endif 

namespace sc2021{enum class ECommandType{Wait,Complete};struct SCommand{int m_cellIndex;ECommandType m_commandType;};}
namespace sc2021{struct STurnOutputData{SCommand m_command;};}

#include <memory>
namespace sc2021{class CBotImpl;struct SInitInputData;struct STurnInputData;class CBot{public:CBot();~CBot();STurnOutputData FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData);STurnOutputData Update(STurnInputData const& turnInData);private:std::unique_ptr<CBotImpl> m_impl;};}
namespace sc2021{struct SCommand;std::string ToString(SCommand const& cmd);}

#include <algorithm>
#include <assert.h>
template<typename T, size_t TSize>class CVectorInPlace{public:CVectorInPlace() : m_nxtSlotIndex(0) {}CVectorInPlace(size_t const size) { resize(size); }CVectorInPlace(std::initializer_list<T> const& items){reserve(items.size());std::copy(items.begin(), items.end(), begin());m_nxtSlotIndex = items.size();}inline void push_back(T const& elem) { assert(m_nxtSlotIndex < TSize); m_data[m_nxtSlotIndex++] = elem; }template<typename ... TArgs>inline T& emplace_back(TArgs&& ... args) { m_data[m_nxtSlotIndex++] = std::move(T(std::forward<TArgs>(args)...)); return back(); }inline void pop_back() { m_nxtSlotIndex--; }inline T& back() { return m_data[m_nxtSlotIndex - 1]; }inline T& front() { return *m_data; }inline T const& back() const { return m_data[m_nxtSlotIndex - 1]; }inline T const& front() const { return *m_data; }inline size_t size() const { return m_nxtSlotIndex; }inline void resize(size_t const capacity) { m_nxtSlotIndex = capacity; }inline void reserve(size_t const capacity) { assert(capacity <= TSize); }inline size_t max_capacity() const { return TSize; }inline bool is_full() const { return m_nxtSlotIndex >= TSize; }void erase_swap(T* const iter){size_t const index = std::distance(m_data, iter);std::swap(m_data[index], m_data[m_nxtSlotIndex - 1]);pop_back();}inline bool empty() const { return m_nxtSlotIndex == 0; }inline void clear() { m_nxtSlotIndex = 0; }inline T* begin() { return m_data; }inline T* end() { return m_data + m_nxtSlotIndex; }inline T const* begin() const { return m_data; }inline T const* end() const { return m_data + m_nxtSlotIndex; }inline T& operator[](size_t const index) { assert(index < m_nxtSlotIndex); return m_data[index]; }inline T const& operator[](size_t const index) const { assert(index < m_nxtSlotIndex); return m_data[index]; }private:size_t m_nxtSlotIndex = 0;T m_data[TSize];};
#include <vector>
namespace sc2021{struct SInitCellInputData{int m_index;int m_richness;CVectorInPlace<int, MAX_NEIGHS_COUNT> m_neighs;};struct SInitInputData{CVectorInPlace<SInitCellInputData, MAX_CELLS_COUNT> m_cells;};struct STreeInputData{int m_cellIndex;int m_size;bool m_isMine;bool m_isDormant;};struct STurnInputData{int m_day;int m_nutriens;int m_mySun;int m_myScore;int m_oppSun;int m_oppScore;bool m_oppWaiting;CVectorInPlace<STreeInputData, MAX_CELLS_COUNT> m_trees;std::vector<std::string> m_possibleActions;};}

#include <iostream>
using namespace sc2021;using namespace std;void ReadInitInputData(SInitInputData& data){int numberOfCells;cin >> numberOfCells; cin.ignore();cerr << numberOfCells << "\n";data.m_cells.resize(numberOfCells);for (auto& cell : data.m_cells){cin >> cell.m_index>> cell.m_richness;cerr << cell.m_index << " "<< cell.m_richness;cell.m_neighs.resize(MAX_NEIGHS_COUNT);for (auto& neigh : cell.m_neighs){cin >> neigh;cerr << " " << neigh;}cin.ignore();cerr << "\n";}}void ReadTurnInputData(STurnInputData& data){cin >> data.m_day; cin.ignore();cin >> data.m_nutriens; cin.ignore();cin >> data.m_mySun>> data.m_myScore; cin.ignore();cin >> data.m_oppSun>> data.m_oppScore>> data.m_oppWaiting; cin.ignore();int numberOfTrees;cin >> numberOfTrees; cin.ignore();data.m_trees.resize(numberOfTrees);for (auto& tree : data.m_trees){cin >> tree.m_cellIndex;cin >> tree.m_size;cin >> tree.m_isMine;cin >> tree.m_isDormant; cin.ignore();}int numberOfPossibleActions;cin >> numberOfPossibleActions; cin.ignore();data.m_possibleActions.resize(numberOfPossibleActions);for (auto& action : data.m_possibleActions){getline(cin, action);}}void WriteTurnOutputData(STurnOutputData const& data){cout << ToString(data.m_command) << "\n";fflush(stderr);fflush(stdout);}int main(){SInitInputData initInputData;STurnInputData turnInputData;STurnOutputData turnOutputData;CBot bot;ReadInitInputData(initInputData);ReadTurnInputData(turnInputData);{PROFILE_TIME("First update");turnOutputData = bot.FirstUpdate(initInputData, turnInputData);}WriteTurnOutputData(turnOutputData);while (true){ReadTurnInputData(turnInputData);{PROFILE_TIME("Update");turnOutputData = bot.Update(turnInputData);}WriteTurnOutputData(turnOutputData);}return 0;}

#define _CRT_SECURE_NO_WARNINGS

#include <array>
#include <queue>

namespace sc2021{std::string ToString(SCommand const& cmd){char buff[256];switch (cmd.m_commandType){case ECommandType::Wait:sprintf(buff, "WAIT");break;case ECommandType::Complete:sprintf(buff, "COMPLETE %i", cmd.m_cellIndex);break;default:assert(false);}return buff;}}
namespace sc2021{class CBotImpl{public:STurnOutputData FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData);STurnOutputData Update(STurnInputData const& turnInData);private:SInitInputData m_initData;STurnInputData m_turnData;private:SCommand FindTurn();};}
using namespace std;namespace sc2021{CBot::CBot() : m_impl(make_unique<CBotImpl>()) {}CBot::~CBot() {}STurnOutputData CBot::FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData){return m_impl->FirstUpdate(initInData, turnInData);}STurnOutputData CBot::Update(STurnInputData const& turnInData){return m_impl->Update(turnInData);}}
unsigned int UpdateRandomSeed();unsigned int GetRandomUInt(unsigned int const minVal, unsigned int const maxVal);template<typename TIter>TIter GetRandomItem(TIter first, TIter last){size_t const rangeSize = static_cast<size_t>(last - first);return first + GetRandomUInt(0, static_cast<unsigned int>(rangeSize));}template<typename TEnum>TEnum GetRandomEnumValue(){return static_cast<TEnum>(GetRandomUInt(0, static_cast<unsigned int>(TEnum::Count)));}
using namespace std;namespace sc2021{namespace CBotImplPriv{SCommand CreateWaitCmd(){SCommand cmd;cmd.m_commandType = ECommandType::Wait;return cmd;}SCommand CreateCompleteCmd(int const cellIndex){SCommand cmd;cmd.m_cellIndex = cellIndex;cmd.m_commandType = ECommandType::Complete;return cmd;}}STurnOutputData CBotImpl::FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData){m_initData = initInData;int const seed = UpdateRandomSeed();cerr << "Bot seed: " << seed << "\n";return Update(turnInData);}STurnOutputData CBotImpl::Update(STurnInputData const& turnInData){m_turnData = turnInData;STurnOutputData outputData;outputData.m_command = FindTurn();return outputData;}SCommand CBotImpl::FindTurn(){using namespace CBotImplPriv;int const sunPreCompleteCmd = 4;int const maxTreeSize = 3;if (m_turnData.m_mySun < sunPreCompleteCmd){return CreateWaitCmd();}auto findCell = [this](int const cellIndex){return std::find_if(m_initData.m_cells.begin(), m_initData.m_cells.end(), [cellIndex](auto const& cell){return cellIndex == cell.m_index;});};SInitCellInputData* curCell = nullptr;for (auto const& tree : m_turnData.m_trees){if (tree.m_isMine && tree.m_size == maxTreeSize){auto const cell = findCell(tree.m_cellIndex);if (curCell == nullptr || curCell->m_richness < cell->m_richness){curCell = cell;}}}if (!curCell || (curCell->m_richness <= 1 && m_turnData.m_nutriens == 0)){return CreateWaitCmd();}return CreateCompleteCmd(curCell->m_index);}}
#include <random>
#include <time.h>
unsigned int UpdateRandomSeed(){unsigned int const seed = (unsigned int)time(nullptr);srand(seed);return seed;}unsigned int GetRandomUInt(unsigned int const minVal, unsigned int const maxVal){assert(minVal < maxVal);unsigned int const range = maxVal - minVal;return rand() % range + minVal;}
using namespace std;CTimeProfiler::CTimeProfiler(char const* const name): m_name(name), m_startTime(chrono::high_resolution_clock::now()){}CTimeProfiler::~CTimeProfiler(){auto const end = chrono::high_resolution_clock::now();double const executionTime = (double)chrono::duration_cast<chrono::nanoseconds>(end - m_startTime).count();cerr.precision(3);cerr << m_name << ": " << executionTime / 1000000.0 << fixed << " ms" << endl;}
