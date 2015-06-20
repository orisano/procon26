#pragma once
#include "Square.hpp"
#include <cstdint>

namespace procon26 {

struct Tile : Square<std::uint16_t, 8> {
  typedef Square<std::uint16_t, 8> Derived;

  cell_type cell_value;

  Tile();
  void fill(cell_type c);
  cell_type at(int x, int y) const;
};

}
