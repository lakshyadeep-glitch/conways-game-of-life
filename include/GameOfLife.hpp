#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class GameOfLife {
public:
    enum class Pattern {
        Glider,
        Pulsar,
        GosperGliderGun
    };

    GameOfLife(int columns = 64, int rows = 40);

    int columns() const noexcept;
    int rows() const noexcept;
    bool isAlive(int x, int y) const;
    void setCell(int x, int y, bool alive);
    void toggleCell(int x, int y);
    void clear();
    void randomize(float aliveChance = 0.28F);
    void step();
    std::size_t population() const noexcept;
    std::size_t generation() const noexcept;
    void resetGeneration() noexcept;

    // Resizes the grid and keeps cells that still fit in the new dimensions.
    void resize(int newColumns, int newRows);
    void placePattern(Pattern pattern, int centerX, int centerY);

private:
    int columns_;
    int rows_;
    std::vector<std::uint8_t> cells_;
    std::vector<std::uint8_t> nextCells_;
    std::size_t generation_ = 0;

    std::size_t indexOf(int x, int y) const noexcept;
    int countNeighbors(int x, int y) const noexcept;
};
