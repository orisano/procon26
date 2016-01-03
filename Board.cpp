#include "Board.hpp"
#include <cassert>
#include <algorithm>

namespace {
const int dx[] = {1, -1, 0, 0};
const int dy[] = {0, 0, -1, 1};
const int DEFAULT_MIN = 512;
}

namespace procon26 {
namespace board {

Board::Board() : Derived(), min_cell_(DEFAULT_MIN) {}

void Board::put(const tile_type &tile, int x, int y) {
  assert(canPut(tile, x, y));

  auto cv = tile.cell_value;
  for (auto yi = 0; yi < tile_type::SIZE; ++yi) {
    for (auto xi = 0; xi < tile_type::SIZE; ++xi) {
      if (tile.data[yi][xi] == 0) continue;

      const auto nx = x + xi, ny = y + yi;
      data[ny][nx] = cv;
    }
  }
  min_cell_ = std::min<cell_type>(min_cell_, cv);
  zk += tile.zk;
}

bool Board::canPut(const tile::Tile &tile, int x, int y) const {
  if (blanks() < tile.zk) return false;
  if (tile.canSkip(x, y)) return false;

  cell_type min_cv = DEFAULT_MIN - 1;
  for (auto yi = 0; yi < tile::Tile::SIZE; ++yi) {
    for (auto xi = 0; xi < tile::Tile::SIZE; ++xi) {
      if (tile.data[yi][xi] == 0) continue;

      const auto cx = x + xi, cy = y + yi;
      if (!inBounds(cx, cy)) {
        tile.setSkip(x, y);
        return false;
      }
      if (data[cy][cx] != 0) {
        if (data[cy][cx] == 1) tile.setSkip(x, y);
        return false;
      }

      for (auto d = 0; d < 4; d++) {
        const auto nx = cx + dx[d], ny = cy + dy[d];
        if (!inBounds(nx, ny)) continue;

        const auto c = data[ny][nx];
        if (c <= 1) continue;

        min_cv = std::min<cell_type>(min_cv, c);
      }
    }
  }
  return min_cell_ == DEFAULT_MIN || min_cv == min_cell_ ||
         min_cv < tile.cell_value;
}
}
}
