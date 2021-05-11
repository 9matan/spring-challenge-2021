#pragma once

constexpr int MAX_DAYS_COUNT = 33;
constexpr int LAST_DAY_NUMBER = 23;

constexpr int MAX_CELLS_COUNT = 37;
constexpr int MAX_CELL_RICHNESS = 3;
constexpr int CELL_RICHNESS_SCORE[MAX_CELL_RICHNESS + 1] = { 0, 0, 2, 4 };

constexpr int MAX_TREE_SIZE = 3;
constexpr int COMPLETE_LIFECYCLE_PRICE = 4;
constexpr int TREE_LEVEL_UPGRADE_BASE_PRICE[MAX_TREE_SIZE] = {1, 3, 7};
