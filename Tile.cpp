#include "Tile.hpp"
#include <cassert>

namespace procon26 {

Tile::Tile(): cell_value(1)
{}

void Tile::fill(cell_type c)
{
  cell_value = c;
}

Tile::Derived::cell_type Tile::at(int x, int y) const
{
  assert(inBounds(x, y));
  return data[y][x] * cell_value;
}

}