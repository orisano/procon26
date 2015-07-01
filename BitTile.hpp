#pragma once

#include <cstdint>

#include "Tile.hpp"

namespace procon26 {
namespace tile {

struct BitTile {
    using cell_type = std::uint32_t;
    using mask_type = std::uint64_t;
    using size_type = std::uint16_t;

    static const size_type SIZE = 8;
    static const size_type MASK_SIZE = SIZE + 2;

    cell_type data[SIZE];
    mask_type mask[MASK_SIZE];
    size_type zk;

    BitTile();

    explicit BitTile(const Tile &tile);

    void reverse();

    void rotate();

    void fill(int c);

    cell_type at(int x, int y) const;

    bool inBounds(int x, int y) const;

  private:
    void buildMask();
};

}
}