
#if defined(__GNUC__) || defined(__GNUG__)
#pragma GCC optimize("O3")
#pragma GCC optimize("inline")
#pragma GCC optimize("omit-frame-pointer")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("Ofast")
#endif 
#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

constexpr int MAX_DAYS_COUNT = 33;constexpr int LAST_DAY_NUMBER = 23;constexpr int MAX_CELLS_COUNT = 37;constexpr int MAX_CELL_RICHNESS = 3;constexpr int CELL_RICHNESS_SCORE[MAX_CELL_RICHNESS + 1] = { 0, 0, 2, 4 };constexpr int MAX_TREE_SIZE = 3;constexpr int COMPLETE_LIFECYCLE_PRICE = 4;constexpr int TREE_LEVEL_UPGRADE_BASE_PRICE[MAX_TREE_SIZE] = {1, 3, 7};

#include <chrono>
#include <ctime>
#include <string>
class CTimeProfiler{public:CTimeProfiler(char const* const name);~CTimeProfiler();private:std::string m_name;decltype(std::chrono::high_resolution_clock::now()) m_startTime;};
#ifndef DISABLE_PROFILE
#define PROFILE_TIME(name) CTimeProfiler timeProfiler(name);
#else
#define PROFILE_TIME(name)
#endif 

namespace sc2021{enum class ECommandType{Invalid = 0,Wait,Complete,Grow,Seed};struct SCommand{int m_cellIndex;int m_seedCellIndex;ECommandType m_commandType = ECommandType::Invalid;inline void Invalidate() { m_commandType = ECommandType::Invalid; }inline bool IsValid() const { return m_commandType != ECommandType::Invalid; }};
#define INVALID_COMMAND SCommand()
}
namespace sc2021{struct STurnOutputData{SCommand m_command;};}

#include <memory>
namespace sc2021{class CBotImpl;struct SInitInputData;struct STurnInputData;class CBot{public:CBot();~CBot();STurnOutputData FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData);STurnOutputData Update(STurnInputData const& turnInData);private:std::unique_ptr<CBotImpl> m_impl;};}
namespace sc2021{char const* ToString(SCommand const& cmd);SCommand CreateWaitCmd();SCommand CreateCompleteCmd(int const cellIndex);SCommand CreateGrowCmd(int const cellIndex);SCommand CreateSeedCmd(int const cellIndex, int const seedCellIndex);}
namespace sc2021{enum class EDirection{Right = 0,UpRight,UpLeft,Left,DownLeft,DownRight,Count};constexpr int DIRECTIONS_COUNT = (int)EDirection::Count;inline EDirection GetNextDirection(EDirection const direction) { return (EDirection)(((int)direction + 1) % DIRECTIONS_COUNT); }inline EDirection GetCurrentShadowDirection(int const dayIndex) { return (EDirection)(dayIndex % DIRECTIONS_COUNT); }inline EDirection GetInverseDirection(EDirection const direction) { return (EDirection)(((int)direction + DIRECTIONS_COUNT / 2) % DIRECTIONS_COUNT); }}

#include <algorithm>
#include <assert.h>
template<typename T, size_t TSize>class CVectorInPlace{public:static const size_t MAX_SIZE = TSize;public:CVectorInPlace() : m_nxtSlotIndex(0) {}CVectorInPlace(size_t const size) { resize(size); }CVectorInPlace(std::initializer_list<T> const& items){reserve(items.size());std::copy(items.begin(), items.end(), begin());m_nxtSlotIndex = items.size();}inline void push_back(T const& elem) { assert(m_nxtSlotIndex < TSize); m_data[m_nxtSlotIndex++] = elem; }template<typename ... TArgs>inline T& emplace_back(TArgs&& ... args) { m_data[m_nxtSlotIndex++] = std::move(T(std::forward<TArgs>(args)...)); return back(); }inline void pop_back() { m_nxtSlotIndex--; }inline T& back() { return m_data[m_nxtSlotIndex - 1]; }inline T& front() { return *m_data; }inline T const& back() const { return m_data[m_nxtSlotIndex - 1]; }inline T const& front() const { return *m_data; }inline size_t size() const { return m_nxtSlotIndex; }inline void resize(size_t const capacity) { m_nxtSlotIndex = capacity; }inline void reserve(size_t const capacity) { assert(capacity <= TSize); }inline size_t max_capacity() const { return TSize; }inline bool is_full() const { return m_nxtSlotIndex >= TSize; }void erase_swap(T* const iter){size_t const index = std::distance(m_data, iter);std::swap(m_data[index], m_data[m_nxtSlotIndex - 1]);pop_back();}inline bool empty() const { return m_nxtSlotIndex == 0; }inline void clear() { m_nxtSlotIndex = 0; }inline T* begin() { return m_data; }inline T* end() { return m_data + m_nxtSlotIndex; }inline T const* begin() const { return m_data; }inline T const* end() const { return m_data + m_nxtSlotIndex; }inline T& operator[](size_t const index) { assert(index < m_nxtSlotIndex); return m_data[index]; }inline T const& operator[](size_t const index) const { assert(index < m_nxtSlotIndex); return m_data[index]; }private:size_t m_nxtSlotIndex = 0;T m_data[TSize];};
#include <vector>
namespace sc2021{struct SInitCellInputData{int m_index;int m_richness;int m_neighs[DIRECTIONS_COUNT];};struct SInitInputData{CVectorInPlace<SInitCellInputData, MAX_CELLS_COUNT> m_cells;};struct STreeInputData{int m_cellIndex;int m_size;bool m_isMine;bool m_isDormant;};struct STurnInputData{int m_day;int m_nutriens;int m_mySun;int m_myScore;int m_oppSun;int m_oppScore;bool m_oppWaiting;CVectorInPlace<STreeInputData, MAX_CELLS_COUNT> m_trees;std::vector<std::string> m_possibleActions;};}

#include <iostream>
using namespace sc2021;using namespace std;void ReadInitInputData(SInitInputData& data){int numberOfCells;cin >> numberOfCells; cin.ignore();cerr << numberOfCells << "\n";data.m_cells.resize(numberOfCells);for (auto& cell : data.m_cells){cin >> cell.m_index>> cell.m_richness;cerr << cell.m_index << " "<< cell.m_richness;for (auto& neigh : cell.m_neighs){cin >> neigh;cerr << " " << neigh;}cin.ignore();cerr << "\n";}}void ReadTurnInputData(STurnInputData& data){cin >> data.m_day; cin.ignore();cin >> data.m_nutriens; cin.ignore();cin >> data.m_mySun>> data.m_myScore; cin.ignore();cin >> data.m_oppSun>> data.m_oppScore>> data.m_oppWaiting; cin.ignore();int numberOfTrees;cin >> numberOfTrees; cin.ignore();data.m_trees.resize(numberOfTrees);for (auto& tree : data.m_trees){cin >> tree.m_cellIndex;cin >> tree.m_size;cin >> tree.m_isMine;cin >> tree.m_isDormant; cin.ignore();}int numberOfPossibleActions;cin >> numberOfPossibleActions; cin.ignore();data.m_possibleActions.resize(numberOfPossibleActions);for (auto& action : data.m_possibleActions){getline(cin, action);}}void WriteTurnOutputData(STurnOutputData const& data){cout << ToString(data.m_command) << "\n";fflush(stderr);fflush(stdout);}int main(){SInitInputData initInputData;STurnInputData turnInputData;STurnOutputData turnOutputData;CBot bot;ReadInitInputData(initInputData);ReadTurnInputData(turnInputData);{PROFILE_TIME("First update");turnOutputData = bot.FirstUpdate(initInputData, turnInputData);}WriteTurnOutputData(turnOutputData);while (true){ReadTurnInputData(turnInputData);{PROFILE_TIME("Update");turnOutputData = bot.Update(turnInputData);}WriteTurnOutputData(turnOutputData);}return 0;}

#define _CRT_SECURE_NO_WARNINGS
#include <array>
#include <cstring>
#include <functional>
#include <queue>
#include <unordered_map>
#include <utility>
namespace sc2021{char const* ToString(SCommand const& cmd){static char buff[256];switch (cmd.m_commandType){case ECommandType::Invalid:sprintf(buff, "Invalid");break;case ECommandType::Wait:sprintf(buff, "WAIT");break;case ECommandType::Complete:sprintf(buff, "COMPLETE %i", cmd.m_cellIndex);break;case ECommandType::Grow:sprintf(buff, "GROW %i", cmd.m_cellIndex);break;case ECommandType::Seed:sprintf(buff, "SEED %i %i", cmd.m_cellIndex, cmd.m_seedCellIndex);break;default:assert(false);}return buff;}SCommand CreateWaitCmd(){SCommand cmd;cmd.m_commandType = ECommandType::Wait;return cmd;}SCommand CreateCompleteCmd(int const cellIndex){SCommand cmd;cmd.m_cellIndex = cellIndex;cmd.m_commandType = ECommandType::Complete;return cmd;}SCommand CreateGrowCmd(int const cellIndex){SCommand cmd;cmd.m_cellIndex = cellIndex;cmd.m_commandType = ECommandType::Grow;return cmd;}SCommand CreateSeedCmd(int const cellIndex, int const seedCellIndex){SCommand cmd;cmd.m_cellIndex = cellIndex;cmd.m_seedCellIndex = seedCellIndex;cmd.m_commandType = ECommandType::Seed;return cmd;}}
namespace sc2021{struct STreeEntity{int m_size = -1;bool m_isMine;bool m_isDormant;inline void Invalidate() { m_size = -1; }inline bool IsValid() const { return m_size != -1; }};
#define INVALID_TREE_ENTITY STreeEntity()
}
namespace sc2021{struct STreeEntity;struct SCellEntity{STreeEntity m_tree;int m_index;int m_richness;SCellEntity* m_neigh[DIRECTIONS_COUNT] = { 0 };inline bool HasMyTree_Dormant(bool const isDormant) const { return HasMyTree() && m_tree.m_isDormant == isDormant; }inline bool HasMyTree() const { return m_tree.IsValid() && m_tree.m_isMine; }inline bool HasTree() const { return m_tree.IsValid(); }};}
namespace sc2021{class CMap{public:struct SCellEntry{SCellEntry() : SCellEntry(nullptr) {}SCellEntry(SCellEntity* cell, int distance = -1): m_cell(cell), m_distance(distance){}SCellEntity* m_cell;int m_distance;};public:using CellPredicate = std::function<bool(SCellEntity const&)>;using Cells = CVectorInPlace<SCellEntity*, MAX_CELLS_COUNT>;using CellEntries = CVectorInPlace<SCellEntry, MAX_CELLS_COUNT>;public:void GetCells(Cells& cells, CellPredicate predicate);void GetCellsInRadius(CellEntries& cells, int const cellIndex, int const radius);void GetCellsInDirection(CellEntries& cells, int const cellIndex, EDirection const direction, int const maxDistance);public:inline SCellEntity* begin() { return std::begin(m_cells); }inline SCellEntity* end() { return std::end(m_cells); }inline SCellEntity& operator[](size_t const index) { return m_cells[index]; }inline SCellEntity const* begin() const { return std::begin(m_cells); }inline SCellEntity const* end() const { return std::end(m_cells); }inline SCellEntity const& operator[](size_t const index) const { return m_cells[index]; }private:SCellEntity m_cells[MAX_CELLS_COUNT];};}
namespace sc2021{constexpr int SM_MAX_DAYS_TO_SIMULATE = 6;class CShadowManager{public:using DayCoefCalculator = std::function<float(int startDay, int endDay, int curDay)>;public:void UpdateDarknessLevel(CMap& map, EDirection const currentShadowDirection, int const daysToSimulate);float GetDarknessLevelInRange_Avrg(int const cellIndex, int const startDay, int const endDay, int const treeLevel = 0);float GetDarknessLevelInRange(int const cellIndex, int const startDay, int const endDay, DayCoefCalculator dayCoefCalculator, int const treeLevel = 0);private:int m_numberOfSimulatedDays;float m_darknessLevel[SM_MAX_DAYS_TO_SIMULATE + 1][MAX_CELLS_COUNT][MAX_TREE_SIZE + 1];private:float CalculateCellDarknessLevel(int const treeLevel, int const day, CMap::CellEntries const& entries);};}
constexpr int MAX_TURNS_PER_DAY = 16;namespace sc2021{enum class ETurnStrategyType{Invalid = 0,CompleteLifeCycle,IncreaseIncome,UpgradeToLargeTree,SeedNewTree};struct STurnStrategy{using Condition = std::function<bool(STurnStrategy const&)>;ETurnStrategyType m_strategyType = ETurnStrategyType::Invalid;int m_iterationsCount = 1;Condition m_condition = nullptr;inline void Invalidate() { m_strategyType = ETurnStrategyType::Invalid; }inline bool IsValid() const { return m_strategyType == ETurnStrategyType::Invalid; }STurnStrategy& SetCondition(Condition condition) { m_condition = condition; return *this; }};constexpr int INFINITY_ITERATIONS_COUNT = -1;
#define INVALID_TURN_STRATEGY STurnStrategy()
struct SDayStrategy{CVectorInPlace<STurnStrategy, MAX_TURNS_PER_DAY> m_turnStrategies;bool m_isValid = false;inline void Invalidate() { m_turnStrategies.clear(); m_isValid = false; }inline bool IsValid() const { return !m_turnStrategies.empty() || m_isValid; }inline void Validate() { m_isValid = true; }};
#define INVALID_DAY_STRATEGY SDayStrategy()
}
namespace sc2021{class CBotImpl{public:STurnOutputData FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData);STurnOutputData Update(STurnInputData const& turnInData);private:STurnInputData m_turnData;std::unordered_map<int, SDayStrategy> m_predefinedDayStrategies;SDayStrategy m_defaultDayStrategy;SDayStrategy m_currentDayStrategy;CMap m_map;CShadowManager m_shadowManager;int m_myTreesCntBySize[MAX_TREE_SIZE + 1];int m_oppTreesCntBySize[MAX_TREE_SIZE + 1];private:void InitMap(SInitInputData const& initData);void UpdateTrees();void UpdateShadowManager();SDayStrategy CalculateDayStrategy();SCommand FindTurn();SCommand FindTurnByStrategy(STurnStrategy const& turnStrategy);SCommand FindTurn_CompleteLifeCycle(STurnStrategy const& turnStrategy);SCommand FindTurn_IncreaseIncome(STurnStrategy const& turnStrategy);SCommand FindTurn_UpgradeToLargeTree(STurnStrategy const& turnStrategy);SCommand FindTurn_SeedNewTree(STurnStrategy const& turnStrategy);void OnNewDay();private:inline int GetUpgradePrice(int const treeSize) const{assert("INVALID tree size" && treeSize >= 0 && treeSize < MAX_TREE_SIZE);return TREE_LEVEL_UPGRADE_BASE_PRICE[treeSize] + m_myTreesCntBySize[treeSize + 1];}inline int GetSeedPrice() const { return m_myTreesCntBySize[0];}inline int GetMySun() const { return m_turnData.m_mySun; }inline int GetMyScore() const { return m_turnData.m_myScore; }inline int GetDaysRemaining() const { return LAST_DAY_NUMBER - GetCurrentDay(); }inline int GetCurrentDay() const { return m_turnData.m_day; }inline int GetNutriens() const { return m_turnData.m_nutriens; }inline int GetTreesCount(int const treeSize) const{assert(treeSize <= MAX_TREE_SIZE && treeSize >= -1);if (treeSize == -1){int res = 0;for (int i = 0; i <= MAX_TREE_SIZE; ++i)res += GetTreesCount(i);return res;}return m_myTreesCntBySize[treeSize] + m_oppTreesCntBySize[treeSize];}};}
using namespace std;namespace sc2021{CBot::CBot() : m_impl(make_unique<CBotImpl>()) {}CBot::~CBot() {}STurnOutputData CBot::FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData){return m_impl->FirstUpdate(initInData, turnInData);}STurnOutputData CBot::Update(STurnInputData const& turnInData){return m_impl->Update(turnInData);}}
class CRanges{public:CRanges(std::initializer_list<int> const& rangePoints){assert(std::is_sorted(rangePoints.begin(), rangePoints.end()));m_rangesPoints.assign(rangePoints.begin(), rangePoints.end());}inline size_t GetRangeIndex(int const value) const { return std::upper_bound(m_rangesPoints.begin(), m_rangesPoints.end(), value) - m_rangesPoints.begin(); }inline size_t RangesCount() const { return m_rangesPoints.size() + 1; }private:std::vector<int> m_rangesPoints;};template<size_t RangesCount, typename TFunc>class CCompositeRangeFunction{public:using InRangesPoints = CVectorInPlace<int, RangesCount - 1>;using InFunctions = CVectorInPlace<TFunc, RangesCount>;public:CCompositeRangeFunction(InRangesPoints const& ranges, InFunctions const& functions){assert(InRangesPoints::MAX_SIZE + 1 == InFunctions::MAX_SIZE);assert(ranges.is_full());assert(functions.is_full());assert(std::is_sorted(ranges.begin(), ranges.end()));std::copy(ranges.begin(), ranges.end(), m_ranges);std::copy(functions.begin(), functions.end(), m_functions);}size_t GetRangeIndex(int const value) const { return std::upper_bound(m_ranges, m_ranges + InRangesPoints::MAX_SIZE, value) - m_ranges;  }TFunc& GetFunctionByValue(int const value) { return m_functions[GetRangeIndex(value)]; }private:int m_ranges[InRangesPoints::MAX_SIZE];TFunc m_functions[RangesCount];};
unsigned int UpdateRandomSeed();unsigned int GetRandomUInt(unsigned int const minVal, unsigned int const maxVal);template<typename TIter>TIter GetRandomItem(TIter first, TIter last){size_t const rangeSize = static_cast<size_t>(last - first);return first + GetRandomUInt(0, static_cast<unsigned int>(rangeSize));}template<typename TEnum>TEnum GetRandomEnumValue(){return static_cast<TEnum>(GetRandomUInt(0, static_cast<unsigned int>(TEnum::Count)));}
namespace sc2021{char const* ToString(STurnStrategy const& turnStrategy);STurnStrategy CreateCompleteLifeCycleTS(int const iterationsCount = 1);STurnStrategy CreateIncreaseIncomeTS(int const iterationsCount = 1);STurnStrategy CreateUpgradeToLargeTreeTS(int const iterationsCount = 1);STurnStrategy CreateSeedNewTreeTS(int const iterationsCount = 1);}
using namespace std;namespace BotConfigs{constexpr int SM_MAX_DAYS_TO_SIMULATE = 6;}namespace sc2021{STurnOutputData CBotImpl::FirstUpdate(SInitInputData const& initInData, STurnInputData const& turnInData){int const seed = UpdateRandomSeed();cerr << "Bot seed: " << seed << "\n";InitMap(initInData);{int nxtDay = 0;{auto& day = m_predefinedDayStrategies[nxtDay++];day.Validate();}{auto& day = m_predefinedDayStrategies[nxtDay++];day.m_turnStrategies.push_back(CreateIncreaseIncomeTS());}{auto& day = m_predefinedDayStrategies[nxtDay++];day.m_turnStrategies.push_back(CreateSeedNewTreeTS());day.m_turnStrategies.push_back(CreateIncreaseIncomeTS());}{auto& preLastDay = m_predefinedDayStrategies[LAST_DAY_NUMBER - 1];preLastDay.m_turnStrategies.push_back(CreateCompleteLifeCycleTS(INFINITY_ITERATIONS_COUNT));preLastDay.m_turnStrategies.push_back(CreateUpgradeToLargeTreeTS(INFINITY_ITERATIONS_COUNT));}{auto& lastDay = m_predefinedDayStrategies[LAST_DAY_NUMBER];lastDay.m_turnStrategies.emplace_back(CreateCompleteLifeCycleTS(INFINITY_ITERATIONS_COUNT));}m_defaultDayStrategy.m_turnStrategies.push_back(CreateCompleteLifeCycleTS().SetCondition([this](auto const&) {if (GetCurrentDay() < 15)return m_myTreesCntBySize[MAX_TREE_SIZE] > 4;if (GetCurrentDay() < 20)return m_myTreesCntBySize[MAX_TREE_SIZE] > 3;return m_myTreesCntBySize[MAX_TREE_SIZE] >= 2;}));m_defaultDayStrategy.m_turnStrategies.push_back(CreateIncreaseIncomeTS(INFINITY_ITERATIONS_COUNT).SetCondition([this](auto const&) {return GetCurrentDay() & 1;}));m_defaultDayStrategy.m_turnStrategies.push_back(CreateUpgradeToLargeTreeTS(INFINITY_ITERATIONS_COUNT).SetCondition([this](auto const&) {return !(GetCurrentDay() & 1);}));m_defaultDayStrategy.m_turnStrategies.push_back(CreateSeedNewTreeTS().SetCondition([this](auto const&) {return m_myTreesCntBySize[0] == 0 && ((GetCurrentDay() % 4) != 0);}));}m_turnData.m_day = -1;return Update(turnInData);}void CBotImpl::InitMap(SInitInputData const& initInData){for (auto const& cellData : initInData.m_cells){auto& cell = m_map[cellData.m_index];cell.m_index = cellData.m_index;cell.m_richness = cellData.m_richness;cell.m_tree.Invalidate();int direction = 0;for (int neighIndex : cellData.m_neighs){if (neighIndex >= 0){cell.m_neigh[direction] = m_map.begin() + neighIndex;}++direction;}}}STurnOutputData CBotImpl::Update(STurnInputData const& turnInData){bool const isNewDay = turnInData.m_day != m_turnData.m_day;m_turnData = turnInData;UpdateTrees();UpdateShadowManager();if (isNewDay){OnNewDay();}STurnOutputData outputData;outputData.m_command = FindTurn();return outputData;}void CBotImpl::UpdateTrees(){memset(m_myTreesCntBySize, 0, sizeof(m_myTreesCntBySize));memset(m_oppTreesCntBySize, 0, sizeof(m_oppTreesCntBySize));CVectorInPlace<int, MAX_CELLS_COUNT> indicesWithTree;for (auto const& treeData : m_turnData.m_trees){indicesWithTree.push_back(treeData.m_cellIndex);auto& cell = m_map[treeData.m_cellIndex];cell.m_tree.m_size = treeData.m_size;cell.m_tree.m_isMine = treeData.m_isMine;cell.m_tree.m_isDormant = treeData.m_isDormant;if (treeData.m_isMine){++m_myTreesCntBySize[treeData.m_size];}else{++m_oppTreesCntBySize[treeData.m_size];}}for (auto& cell : m_map){if (find(indicesWithTree.begin(), indicesWithTree.end(), cell.m_index) == indicesWithTree.end()){cell.m_tree.Invalidate();}}}void CBotImpl::UpdateShadowManager(){PROFILE_TIME("UpdateShadowManager");int const daysToSimulate = min(BotConfigs::SM_MAX_DAYS_TO_SIMULATE, GetDaysRemaining());m_shadowManager.UpdateDarknessLevel(m_map, GetCurrentShadowDirection(GetCurrentDay()), daysToSimulate);}void CBotImpl::OnNewDay(){if (m_predefinedDayStrategies.find(GetCurrentDay()) == m_predefinedDayStrategies.end()){m_currentDayStrategy = CalculateDayStrategy();if (!m_currentDayStrategy.IsValid()){m_currentDayStrategy = m_defaultDayStrategy;}}else{m_currentDayStrategy = m_predefinedDayStrategies[GetCurrentDay()];}reverse(m_currentDayStrategy.m_turnStrategies.begin(), m_currentDayStrategy.m_turnStrategies.end());}SDayStrategy CBotImpl::CalculateDayStrategy(){static const CRanges gamePeriods{ 6 };size_t const gamePeriod = gamePeriods.GetRangeIndex(GetCurrentDay());SDayStrategy strategy = INVALID_DAY_STRATEGY;if (gamePeriod == 0){strategy.m_turnStrategies.push_back(CreateUpgradeToLargeTreeTS());strategy.m_turnStrategies.push_back(CreateIncreaseIncomeTS());}else if (gamePeriod == 1){static const CRanges largeTreeRanges{ 15, 20 };static const CVectorInPlace<int, 3> largeTreeMaxCounts{ 5, 4, 2 };assert(largeTreeMaxCounts.size() == largeTreeRanges.RangesCount());size_t const largeTreeRange = largeTreeRanges.GetRangeIndex(GetCurrentDay());int const largeTreeMaxCount = largeTreeMaxCounts[largeTreeRange];int const largeTreeCurrentCount = m_myTreesCntBySize[MAX_TREE_SIZE];{if (largeTreeCurrentCount >= largeTreeMaxCount){strategy.m_turnStrategies.push_back(CreateCompleteLifeCycleTS());}else if (largeTreeCurrentCount == largeTreeMaxCount - 1){bool const canUpgradeToLarge = ((GetUpgradePrice(2) - 1 + COMPLETE_LIFECYCLE_PRICE) <= GetMySun()) && (m_myTreesCntBySize[MAX_TREE_SIZE - 1] > 0);if (canUpgradeToLarge){strategy.m_turnStrategies.push_back(CreateCompleteLifeCycleTS());strategy.m_turnStrategies.push_back(CreateUpgradeToLargeTreeTS());}}}{auto const upgradeToLargeTreeTS = CreateUpgradeToLargeTreeTS().SetCondition([this, largeTreeMaxCount](auto const&) {return m_myTreesCntBySize[MAX_TREE_SIZE] < largeTreeMaxCount;});auto const increaseIncomeTS = CreateIncreaseIncomeTS();strategy.m_turnStrategies.push_back(upgradeToLargeTreeTS);for (int i = 0; i < 4; ++i){if (GetCurrentDay() & 1){strategy.m_turnStrategies.push_back(increaseIncomeTS);strategy.m_turnStrategies.push_back(upgradeToLargeTreeTS);}else{strategy.m_turnStrategies.push_back(upgradeToLargeTreeTS);strategy.m_turnStrategies.push_back(increaseIncomeTS);}}}}strategy.m_turnStrategies.push_back(CreateSeedNewTreeTS().SetCondition([this](auto const&) {return m_myTreesCntBySize[0] == 0;}));return strategy;}SCommand CBotImpl::FindTurn(){static int iterationsCount = 0;SCommand cmd;auto& turns = m_currentDayStrategy.m_turnStrategies;while (!turns.empty() && !cmd.IsValid()){++iterationsCount;auto const& turn = turns.back();string logLine = ToString(turn);logLine += " Iteration: " + to_string(iterationsCount);bool removeTurn = false;if (turn.m_condition && !turn.m_condition(turn)){logLine += " -> Skip";removeTurn = true;}else{cmd = FindTurnByStrategy(turn);if (!cmd.IsValid()){logLine += " -> Fail";removeTurn = true;}else{logLine += " -> Success";if (turn.m_iterationsCount != INFINITY_ITERATIONS_COUNT&& iterationsCount >= turn.m_iterationsCount){removeTurn = true;}}}cerr << logLine << "\n";if (removeTurn){iterationsCount = 0;turns.pop_back();}}return cmd.IsValid() ? cmd : CreateWaitCmd();}SCommand CBotImpl::FindTurnByStrategy(STurnStrategy const& turnStrategy){switch (turnStrategy.m_strategyType){case ETurnStrategyType::CompleteLifeCycle:return FindTurn_CompleteLifeCycle(turnStrategy);case ETurnStrategyType::IncreaseIncome:return FindTurn_IncreaseIncome(turnStrategy);case ETurnStrategyType::UpgradeToLargeTree:return FindTurn_UpgradeToLargeTree(turnStrategy);case ETurnStrategyType::SeedNewTree:return FindTurn_SeedNewTree(turnStrategy);default:assert(!"Unhandled ETurnStrategyType");}assert(false);return INVALID_COMMAND;}SCommand CBotImpl::FindTurn_CompleteLifeCycle(STurnStrategy const& turnStrategy){if (m_turnData.m_mySun < COMPLETE_LIFECYCLE_PRICE){return INVALID_COMMAND;}SCellEntity const* curCell = nullptr;for (auto const& cell : m_map){if (cell.HasMyTree_Dormant(false) && cell.m_tree.m_size == MAX_TREE_SIZE){if (curCell == nullptr || curCell->m_richness < cell.m_richness){curCell = &cell;}}}if (!curCell || (curCell->m_richness <= 1 && m_turnData.m_nutriens == 0)){return INVALID_COMMAND;}return CreateCompleteCmd(curCell->m_index);}SCommand CBotImpl::FindTurn_IncreaseIncome(STurnStrategy const& turnStrategy){SCellEntity const* curCell = nullptr;int curUpgradePrice = -1;for (auto const& cell : m_map){if (cell.HasMyTree_Dormant(false) && cell.m_tree.m_size < MAX_TREE_SIZE){int const price = GetUpgradePrice(cell.m_tree.m_size);if (curUpgradePrice == -1|| price < curUpgradePrice|| price == curUpgradePrice && curCell->m_richness < cell.m_richness){curUpgradePrice = price;curCell = &cell;}}}if (curUpgradePrice == -1 || GetMySun() < curUpgradePrice){return INVALID_COMMAND;}return CreateGrowCmd(curCell->m_index);}SCommand CBotImpl::FindTurn_UpgradeToLargeTree(STurnStrategy const& turnStrategy){CMap::Cells validCells;m_map.GetCells(validCells, [this](SCellEntity const& cell){return cell.HasMyTree_Dormant(false)&& cell.m_tree.m_size < MAX_TREE_SIZE&& (MAX_TREE_SIZE - cell.m_tree.m_size) <= GetDaysRemaining();});SCellEntity const* curCell = nullptr;for (auto const& cell : validCells){if (curCell == nullptr|| curCell->m_tree.m_size < cell->m_tree.m_size|| (curCell->m_tree.m_size == cell->m_tree.m_size && curCell->m_richness < cell->m_richness)){curCell = cell;}}if (curCell == nullptr || GetMySun() < GetUpgradePrice(curCell->m_tree.m_size)){return INVALID_COMMAND;}return CreateGrowCmd(curCell->m_index);}SCommand CBotImpl::FindTurn_SeedNewTree(STurnStrategy const& turnStrategy){static const CRanges ranges{7, 16};if (GetMySun() < GetSeedPrice()){return INVALID_COMMAND;}size_t const gamePeriod = ranges.GetRangeIndex(GetCurrentDay());auto fitnessFunc = [this, gamePeriod](SCellEntity const* cell){float const expectedNutriensCount = (float)GetNutriens();float const richnessScore = ((float)CELL_RICHNESS_SCORE[cell->m_richness] + expectedNutriensCount) / ((float)CELL_RICHNESS_SCORE[MAX_CELL_RICHNESS] + expectedNutriensCount);float const richnessCoef = (float)GetCurrentDay() / LAST_DAY_NUMBER;float const darknessScore = 1.0f - m_shadowManager.GetDarknessLevelInRange_Avrg(cell->m_index, 2, -1);float const darknessCoef = 1.0f - richnessCoef;return richnessScore * richnessCoef + darknessScore * darknessCoef;};SCellEntity const* curCell = nullptr;SCellEntity const* curSeedCell = nullptr;CMap::CellEntries neighCellEntries;for (auto const& cell : m_map){if (cell.HasMyTree_Dormant(false) && cell.m_tree.m_size > 0){m_map.GetCellsInRadius(neighCellEntries, cell.m_index, cell.m_tree.m_size);for (auto const entry : neighCellEntries){auto const neighCell = entry.m_cell;if (neighCell->m_richness == 0) continue;if (neighCell->HasTree()) continue;if (curSeedCell == nullptr|| (fitnessFunc(curSeedCell) < fitnessFunc(neighCell))){curCell = &cell;curSeedCell = neighCell;}}}}if (!curCell){return INVALID_COMMAND;}assert(curCell && curSeedCell);return CreateSeedCmd(curCell->m_index, curSeedCell->m_index);}}
using namespace std;namespace sc2021{void CShadowManager::UpdateDarknessLevel(CMap& map, EDirection const currentShadowDirection, int const daysToSimulate){assert(daysToSimulate <= SM_MAX_DAYS_TO_SIMULATE);for (int day = 0; day <= daysToSimulate; ++day){for (int cellIndex = 0; cellIndex < MAX_CELLS_COUNT; ++cellIndex){for (int treeLevel = 0; treeLevel < MAX_TREE_SIZE; ++treeLevel){m_darknessLevel[day][cellIndex][treeLevel] = 0.0f;}}}CMap::CellEntries cellEntries;EDirection sunDirection = GetInverseDirection(currentShadowDirection);for (int day = 1; day <= daysToSimulate; ++day){sunDirection = GetNextDirection(sunDirection);for (auto const& cell : map){for (int treeLevel = 0; treeLevel < MAX_TREE_SIZE; ++treeLevel){map.GetCellsInDirection(cellEntries, cell.m_index, sunDirection, MAX_TREE_SIZE);float const darkness = CalculateCellDarknessLevel(treeLevel, day, cellEntries);if (darkness > 1.0f || darkness < 0.0f){cerr << "[ERROR] Invalid darkness level: " << darkness<< " cell index: " << cell.m_index<< " cell neigh: " << cellEntries.front().m_cell->m_index<< "\n";}m_darknessLevel[day][cell.m_index][treeLevel] = max(min(darkness, 1.0f), 0.0f);;}}}m_numberOfSimulatedDays = daysToSimulate;}float CShadowManager::GetDarknessLevelInRange_Avrg(int const cellIndex, int const startDay, int const endDay, int const treeLevel){auto avrgFunction = [](int startDay, int endDay, int curDay){return 1.0f / (float)(endDay - startDay + 1);};return GetDarknessLevelInRange(cellIndex, startDay, endDay, avrgFunction, treeLevel);}float CShadowManager::GetDarknessLevelInRange(int const cellIndex, int const startDay, int const endDay, DayCoefCalculator dayCoefCalculator, int const treeLevel){if (startDay > m_numberOfSimulatedDays){return 0.0f;}int const validateEndDay = (endDay == -1)? m_numberOfSimulatedDays: min(endDay, m_numberOfSimulatedDays);assert(startDay <= validateEndDay);assert(startDay >= 0);assert(validateEndDay > 0);if (startDay < 1){cerr << "[ERROR] Invalid start day: " << startDay<< " simulation starts from 1st day"<< "\n";}float darkness = 0.0f;for (int day = max(startDay, 1); day <= validateEndDay; ++day){float const currentDayCoef = (dayCoefCalculator) ? dayCoefCalculator(startDay, validateEndDay, day) : 1.0f;darkness += m_darknessLevel[day][cellIndex][treeLevel] * currentDayCoef;}return darkness;}float CShadowManager::CalculateCellDarknessLevel(int const treeLevel, int const day, CMap::CellEntries const& entries){int curLevelToOutshine = -1;int curTreeLevel = -1;for (auto const& cellEntry : entries){if (!cellEntry.m_cell->HasTree()) continue;auto const cell = cellEntry.m_cell;int const levelDiff = max(0, treeLevel - cell->m_tree.m_size);int const distanceToShadow = max(0, cellEntry.m_distance - cell->m_tree.m_size);int const levelToOutshine = max(levelDiff, distanceToShadow);if (curLevelToOutshine == -1|| curLevelToOutshine > levelToOutshine|| (curLevelToOutshine == levelToOutshine && curTreeLevel > cell->m_tree.m_size)){curLevelToOutshine = levelToOutshine;curTreeLevel = cell->m_tree.m_size;}}if (curLevelToOutshine != -1){float darkness = 0.0f;if (curLevelToOutshine > 0){assert(curTreeLevel != -1);float const majorStep = 1.0f / (float)MAX_TREE_SIZE;float const minorStep = majorStep / ((float)MAX_TREE_SIZE * 2.0f);darkness = (curLevelToOutshine <= day)? (MAX_TREE_SIZE - curLevelToOutshine) * majorStep + (MAX_TREE_SIZE - curTreeLevel) * minorStep: 0.0f;}else{darkness = 1.0f;}return darkness;}return 0.0f;}}
namespace sc2021{void CMap::GetCells(Cells& cells, CellPredicate predicate){cells.clear();cells.reserve(MAX_CELLS_COUNT);for (auto& cell : m_cells){if (predicate(cell)){cells.push_back(&cell);}}}void CMap::GetCellsInRadius(CellEntries& cells, int const cellIndex, int const radius){assert(cellIndex >= 0 && cellIndex < MAX_CELLS_COUNT);cells.clear();cells.reserve(MAX_CELLS_COUNT);bool added[MAX_CELLS_COUNT];memset(added, 0, sizeof(added));added[cellIndex] = true;std::queue<std::pair<int, int>> q;q.push(std::make_pair(cellIndex, 0));while (!q.empty()){auto const cur = q.front();int const index = cur.first;int const distance = cur.second;auto& cell = m_cells[index];q.pop();cells.push_back(SCellEntry(&cell, distance));if (distance < radius){for (auto& neigh : cell.m_neigh){if (!neigh) continue;if (added[neigh->m_index]) continue;added[neigh->m_index] = true;q.push(std::make_pair(neigh->m_index, distance + 1));}}}}void CMap::GetCellsInDirection(CellEntries& cells, int const cellIndex, EDirection const direction, int const maxDistance){assert(cellIndex >= 0 && cellIndex < MAX_CELLS_COUNT);cells.clear();cells.reserve(MAX_CELLS_COUNT);int const directionIndex = (int)direction;SCellEntity* curCell = m_cells + cellIndex;int distance = 0;while (true){++distance;if (distance > maxDistance) break;SCellEntity* nxtCell = curCell->m_neigh[directionIndex];if (!nxtCell) break;curCell = nxtCell;cells.push_back(SCellEntry(nxtCell, distance));}}}
namespace sc2021{char const* ToString(STurnStrategy const& turnStrategy){static char buff[256];int curPos = 0;if (!turnStrategy.IsValid()){switch (turnStrategy.m_strategyType){case ETurnStrategyType::CompleteLifeCycle:curPos = sprintf(buff, "CompleteLifeCycle");break;case ETurnStrategyType::IncreaseIncome:curPos = sprintf(buff, "IncreaseIncome");break;case ETurnStrategyType::UpgradeToLargeTree:curPos = sprintf(buff, "UpgradeToLargeTree");break;case ETurnStrategyType::SeedNewTree:curPos = sprintf(buff, "SeedNewTree");break;default:assert(!"Unhandled ETurnStrategyType");}if (turnStrategy.m_iterationsCount == INFINITY_ITERATIONS_COUNT){curPos = sprintf(buff + curPos, " (endless)");}else{curPos = sprintf(buff + curPos, " (x%i)", turnStrategy.m_iterationsCount);}}else{sprintf(buff, "Invalid");}return buff;}STurnStrategy CreateCompleteLifeCycleTS(int const iterationsCount){STurnStrategy strategy;strategy.m_strategyType = ETurnStrategyType::CompleteLifeCycle;strategy.m_iterationsCount = iterationsCount;return strategy;}STurnStrategy CreateIncreaseIncomeTS(int const iterationsCount){STurnStrategy strategy;strategy.m_strategyType = ETurnStrategyType::IncreaseIncome;strategy.m_iterationsCount = iterationsCount;return strategy;}STurnStrategy CreateUpgradeToLargeTreeTS(int const iterationsCount){STurnStrategy strategy;strategy.m_strategyType = ETurnStrategyType::UpgradeToLargeTree;strategy.m_iterationsCount = iterationsCount;return strategy;}STurnStrategy CreateSeedNewTreeTS(int const iterationsCount){STurnStrategy strategy;strategy.m_strategyType = ETurnStrategyType::SeedNewTree;strategy.m_iterationsCount = iterationsCount;return strategy;}}
#include <random>
#include <time.h>
unsigned int UpdateRandomSeed(){unsigned int const seed = (unsigned int)time(nullptr);srand(seed);return seed;}unsigned int GetRandomUInt(unsigned int const minVal, unsigned int const maxVal){assert(minVal < maxVal);unsigned int const range = maxVal - minVal;return rand() % range + minVal;}
using namespace std;namespace{static thread_local int gNestedLevel = 0;}CTimeProfiler::CTimeProfiler(char const* const name): m_name(name), m_startTime(chrono::high_resolution_clock::now()){++gNestedLevel;}CTimeProfiler::~CTimeProfiler(){auto const end = chrono::high_resolution_clock::now();double const executionTime = (double)chrono::duration_cast<chrono::nanoseconds>(end - m_startTime).count();string const tabs(gNestedLevel, '-');cerr.precision(3);cerr << "[Profiler] " << tabs << m_name << ": " << executionTime / 1000000.0 << fixed << " ms" << endl;--gNestedLevel;}
