#include "Board.hpp"
#include <cassert>
#include <algorithm>

namespace {
const int dx[] = {1, -1, 0, 0};
const int dy[] = {0, 0, -1, 1};
}

namespace procon26 {

Board::Board(): min_cell_(512) {}

void Board::put(const Tile& tile, int x, int y)
{
  assert(puttable(tile, x, y));
  int cc = 0;
  auto cv = tile.cell_value;
  for (auto yi = 0; yi < Tile::SIZE; ++yi) {
    for (auto xi = 0; xi < Tile::SIZE; ++xi) {
      if (tile.data[yi][xi] == 0) continue;
      const auto nx = x + xi, ny = y + yi;
      data[ny][nx] = cv;
      cc++;
    }
  }
  min_cell_ = std::min<cell_type>(min_cell_, cv);
  zk -= cc;
}

bool Board::puttable(const Tile& tile, int x, int y) const
{
  auto min_cv = 511;
  for (auto yi = 0; yi < Tile::SIZE; ++yi) {
    for (auto xi = 0; xi < Tile::SIZE; ++xi) {
      if (tile.data[yi][xi] == 0) continue;
      const auto cx = x + xi, cy = y + yi;
      if (!inBounds(cx, cy)) return false;
      if (data[cy][cx] != 0) return false;
      for (auto d = 0; d < 4; d++) {
        const auto nx = cx + dx[d], ny = cy + dy[d];
        if (!inBounds(nx, ny)) continue;
        const auto c = data[ny][nx];
        if (c <= 1) continue;
        min_cv = std::min<cell_type>(min_cv, c);
      }
    }
  }
  return min_cell_ == 512 || min_cv == min_cell_ || min_cv < tile.cell_value;
}

Board::size_type Board::blanks() const {
    return static_cast<size_type>(zk);
}
}
