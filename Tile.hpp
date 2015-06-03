#pragma once
#include "Square.hpp"

namespace procon26 {

struct Tile : Square<int, 8> {
  typedef Square<int, 8> Derived;

  cell_type cell_value;

  Tile();
  void fill(cell_type c);
  cell_type at(int x, int y) const;
};

}