#pragma once
#include "Square.hpp"
#include "Tile.hpp"

namespace procon26 {

struct Board : Square<int, 32> {
  typedef Square<int, 32> Derived;

  Board();
  void put(const Tile& tile, int x, int y);
  bool puttable(const Tile& tile, int x, int y) const;
private:
  cell_type min_cell_;
};

}