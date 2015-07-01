#pragma once
#include "Square.hpp"
#include "Tile.hpp"
#include <cstdint>

namespace procon26 {
namespace board {

struct Board : Square<std::uint16_t, 32> {
    using Derived = Square<std::uint16_t, 32>;
    using size_type = std::uint16_t;

    Board();

    void put(const tile::Tile &tile, int x, int y);

    bool canPut(const tile::Tile &tile, int x, int y) const;

  private:
    cell_type min_cell_;
};

}
}
