#include "GameOfLife.hpp"

#include <algorithm>
#include <random>
#include <stdexcept>
#include <utility>

GameOfLife::GameOfLife(int columns, int rows)
    : columns_(std::max(1, columns)),
      rows_(std::max(1, rows)),
      cells_(static_cast<std::size_t>(columns_) * static_cast<std::size_t>(rows_), 0),
      nextCells_(cells_.size(), 0) {}

int GameOfLife::columns() const noexcept { return columns_; }
int GameOfLife::rows() const noexcept { return rows_; }

std::size_t GameOfLife::indexOf(int x, int y) const noexcept {
    return static_cast<std::size_t>(y) * static_cast<std::size_t>(columns_)
         + static_cast<std::size_t>(x);
}

bool GameOfLife::isAlive(int x, int y) const {
    if (x < 0 || y < 0 || x >= columns_ || y >= rows_) {
        return false;
    }
    return cells_[indexOf(x, y)] != 0;
}

void GameOfLife::setCell(int x, int y, bool alive) {
    if (x < 0 || y < 0 || x >= columns_ || y >= rows_) {
        return;
    }
    cells_[indexOf(x, y)] = alive ? 1 : 0;
}

void GameOfLife::toggleCell(int x, int y) {
    if (x < 0 || y < 0 || x >= columns_ || y >= rows_) {
        return;
    }
    auto& cell = cells_[indexOf(x, y)];
    cell = cell == 0 ? 1 : 0;
}

void GameOfLife::clear() {
    std::fill(cells_.begin(), cells_.end(), 0);
    std::fill(nextCells_.begin(), nextCells_.end(), 0);
    generation_ = 0;
}

void GameOfLife::randomize(float aliveChance) {
    aliveChance = std::clamp(aliveChance, 0.0F, 1.0F);
    std::mt19937 generator(std::random_device{}());
    std::bernoulli_distribution alive(aliveChance);
    for (auto& cell : cells_) {
        cell = alive(generator) ? 1 : 0;
    }
    std::fill(nextCells_.begin(), nextCells_.end(), 0);
    generation_ = 0;
}

int GameOfLife::countNeighbors(int x, int y) const noexcept {
    int count = 0;
    for (int offsetY = -1; offsetY <= 1; ++offsetY) {
        for (int offsetX = -1; offsetX <= 1; ++offsetX) {
            if (offsetX == 0 && offsetY == 0) {
                continue;
            }
            const int neighborX = x + offsetX;
            const int neighborY = y + offsetY;
            if (neighborX >= 0 && neighborY >= 0 &&
                neighborX < columns_ && neighborY < rows_ &&
                cells_[indexOf(neighborX, neighborY)] != 0) {
                ++count;
            }
        }
    }
    return count;
}

void GameOfLife::step() {
    // Read only from cells_; write every result into nextCells_.
    for (int y = 0; y < rows_; ++y) {
        for (int x = 0; x < columns_; ++x) {
            const int neighbors = countNeighbors(x, y);
            const bool alive = cells_[indexOf(x, y)] != 0;
            const bool nextAlive = alive
                ? (neighbors == 2 || neighbors == 3)
                : (neighbors == 3);
            nextCells_[indexOf(x, y)] = nextAlive ? 1 : 0;
        }
    }
    cells_.swap(nextCells_);
    ++generation_;
}

std::size_t GameOfLife::population() const noexcept {
    return static_cast<std::size_t>(
        std::count(cells_.begin(), cells_.end(), static_cast<std::uint8_t>(1)));
}

std::size_t GameOfLife::generation() const noexcept { return generation_; }
void GameOfLife::resetGeneration() noexcept { generation_ = 0; }

void GameOfLife::resize(int newColumns, int newRows) {
    newColumns = std::max(1, newColumns);
    newRows = std::max(1, newRows);
    if (newColumns == columns_ && newRows == rows_) {
        return;
    }

    std::vector<std::uint8_t> resized(
        static_cast<std::size_t>(newColumns) * static_cast<std::size_t>(newRows), 0);
    const int copyColumns = std::min(columns_, newColumns);
    const int copyRows = std::min(rows_, newRows);

    for (int y = 0; y < copyRows; ++y) {
        for (int x = 0; x < copyColumns; ++x) {
            resized[static_cast<std::size_t>(y) * static_cast<std::size_t>(newColumns)
                    + static_cast<std::size_t>(x)] = cells_[indexOf(x, y)];
        }
    }

    columns_ = newColumns;
    rows_ = newRows;
    cells_ = std::move(resized);
    nextCells_.assign(cells_.size(), 0);
}

void GameOfLife::placePattern(Pattern pattern, int centerX, int centerY) {
    std::vector<std::pair<int, int>> points;

    if (pattern == Pattern::Glider) {
        points = {{0, -1}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};
    } else if (pattern == Pattern::Pulsar) {
        // One quadrant of the classic 13x13 Pulsar, mirrored into four quadrants.
        const std::vector<std::pair<int, int>> base = {
            {2, 0}, {3, 0}, {4, 0}, {8, 0}, {9, 0}, {10, 0},
            {0, 2}, {5, 2}, {7, 2}, {12, 2},
            {0, 3}, {5, 3}, {7, 3}, {12, 3},
            {0, 4}, {5, 4}, {7, 4}, {12, 4},
            {2, 5}, {3, 5}, {4, 5}, {8, 5}, {9, 5}, {10, 5}
        };
        for (const auto& [x, y] : base) {
            for (int sx : {-1, 1}) {
                for (int sy : {-1, 1}) {
                    points.emplace_back(sx * x, sy * y);
                }
            }
        }
    } else {
        // Gosper Glider Gun, using the standard 36-cell seed coordinates.
        points = {
            {-18, 0}, {-17, 0}, {-18, 1}, {-17, 1},
            {-8, 0}, {-7, 0}, {-9, 1}, {-8, 1}, {-8, 2},
            {-3, -2}, {-2, -2}, {-3, -1}, {-2, -1},
            {1, -2}, {2, -2}, {1, -1}, {2, -1},
            {4, -3}, {4, -2}, {4, -1},
            {5, -4}, {5, 0}, {6, -5}, {6, -4}, {6, 0}, {6, 1},
            {16, -2}, {17, -2}, {16, -1}, {17, -1},
            {-1, -3}, {-1, -4}, {-2, -4}, {-2, -3},
            {-10, -1}, {-10, -2}
        };
    }

    clear();
    for (const auto& [offsetX, offsetY] : points) {
        setCell(centerX + offsetX, centerY + offsetY, true);
    }
}
