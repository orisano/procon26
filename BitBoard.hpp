#pragma once

#include <cstdint>
#include <cassert>

#include "Board.hpp"
#include "BitTile.hpp"

namespace procon26 {

struct BitBoard {
    using cell_type = std::uint32_t;
    using size_type = std::uint16_t;
    static const size_type SIZE = 32;

    static cell_type initial[SIZE];
    static size_type initial_zk;

    cell_type data[SIZE];
    size_type zk;

    BitBoard();
    explicit BitBoard(const Board& board);
    cell_type at(int x, int y) const;
    bool puttable(const BitTile& tile, int x, int y) const;
    void put(const BitTile& tile, int x, int y);
    size_type blanks() const;
    bool inBounds(int x, int y) const;
};

}