#pragma once
#include "Square.hpp"
#include "Tile.hpp"
#include <cstdint>

namespace procon26 {
namespace board {

struct Board : Square<std::uint16_t, 32> {
  using Derived = Square<std::uint16_t, 32>;
  using size_type = std::uint16_t;
  using tile_type = tile::Tile;

  Board();

  void put(const tile_type &tile, int x, int y);

  bool canPut(const tile_type &tile, int x, int y) const;

 private:
  cell_type min_cell_;
};
}
}
