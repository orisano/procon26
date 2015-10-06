#ifndef INCLUDE_BIT_BOARD_HPP
#define INCLUDE_BIT_BOARD_HPP

#include <cstdint>
#include <cassert>
#include <limits>

#include "Board.hpp"
#include "BitTile.hpp"

namespace procon26 {
namespace board {

struct BitBoard {
  using tile_type = tile::BitTile;
  using cell_type = std::uint32_t;
  using size_type = std::uint16_t;
  using state_type = std::uint8_t;
  static const size_type SIZE = 32;

  static cell_type initial[SIZE];
  static size_type initial_zk;

  cell_type data[SIZE];
  state_type state[SIZE][SIZE];
  state_type mini = std::numeric_limits<state_type>::max();
  size_type zk = 0;

  BitBoard();

  explicit BitBoard(const Board &board);

  cell_type at(int x, int y) const;

  bool canPut(const tile_type &tile, int x, int y) const;
  bool canPutStrong(const tile_type &tile, int x, int y) const;
  void put(const tile_type &tile, int x, int y);

  size_type blanks() const;

  bool inBounds(int x, int y) const;
};
}
}
#endif
