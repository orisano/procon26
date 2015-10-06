#include "BitBoard.hpp"
#include <limits>

namespace {
const int BIT_MARGIN = 10;
}

namespace procon26 {
namespace board {

BitBoard::cell_type BitBoard::initial[BitBoard::SIZE];
BitBoard::size_type BitBoard::initial_zk;

BitBoard::BitBoard() {
  std::memset(initial, 0, sizeof(initial));
  std::memset(data, 0, sizeof(data));
  std::memset(state, 0, sizeof(state));
}

BitBoard::BitBoard(const Board &board) {
  std::memset(initial, 0, sizeof(initial));
  std::memset(data, 0, sizeof(data));
  std::memset(state, 0, sizeof(state));

  const cell_type ONE = 1;
  for (int y = 0; y < SIZE; y++) {
    for (int x = 0; x < SIZE; x++) {
      auto c = board.data[y][x];
      if (!c) continue;
      if (c == 1) {
        initial[y] |= ONE << x;
        initial_zk++;
      } else {
        data[y] |= ONE << x;
        zk++;
      }
    }
  }
}

bool BitBoard::inBounds(int x, int y) const {
  return 0 <= x && x < SIZE && 0 <= y && y < SIZE;
}

BitBoard::cell_type BitBoard::at(int x, int y) const {
  assert(inBounds(x, y));
  if ((initial[y] >> x) & 1) return cell_type(1);
  if ((data[y] >> x) & 1) return static_cast<cell_type>(state[y][x]) + 2;
  return 0;
}

bool BitBoard::canPut(const tile_type &tile, int x, int y) const {
  using mask_type = tile_type::mask_type;
  const mask_type zero = 0;

  if (tile.zk > blanks()) return false;

  mask_type conflict_mask[tile_type::SIZE];
  for (int i = 0; i < tile_type::SIZE; i++) {
    conflict_mask[i] =
        ~((inBounds(0, y + i) ? ~(data[y + i] | initial[y + i]) : zero)
          << BIT_MARGIN);
  }

  mask_type conflict = 0;
  for (int i = 0; i < tile_type::SIZE; i++) {
    conflict |= conflict_mask[i] &
                (static_cast<mask_type>(tile.data[i]) << (BIT_MARGIN + x));
  }
  if (conflict) return false;

  if (!zk) return true;

  mask_type neighbor_mask[tile_type::MASK_SIZE];
  for (int i = 0; i < tile_type::MASK_SIZE; i++) {
    neighbor_mask[i] = (inBounds(0, y + i - 1) ? data[y + i - 1] : zero)
                       << BIT_MARGIN;
  }

  mask_type neighbor = 0;
  for (int i = 0; i < tile_type::MASK_SIZE; i++) {
    neighbor |= neighbor_mask[i] & (tile.mask[i] << (BIT_MARGIN + x - 1));
  }
  return neighbor != zero;
}

bool BitBoard::canPutStrong(const tile_type &tile, int x, int y) const {
  using mask_type = tile_type::mask_type;
  const mask_type zero = 0;

  if (!zk) return true;

  mask_type neighbor_mask[tile_type::MASK_SIZE];
  for (int i = 0; i < tile_type::MASK_SIZE; i++) {
    neighbor_mask[i] = (inBounds(0, y + i - 1) ? data[y + i - 1] : zero)
                       << BIT_MARGIN;
  }

  for (int i = 0; i < tile_type::MASK_SIZE; i++) {
    neighbor_mask[i] &= tile.mask[i] << (BIT_MARGIN + x - 1);
  }

  auto mins = std::numeric_limits<state_type>::max();
  for (int i = 0; i < tile_type::MASK_SIZE; i++) {
    auto v = neighbor_mask[i] >> BIT_MARGIN;
    for (int j; j = __builtin_ffsl(v) - 1, ~j; v &= v - 1) {
    }
  }
  return mins < tile.cell_value || mins == mini;
}

void BitBoard::put(const tile_type &tile, int x, int y) {
  assert(canPut(tile, x, y));
  for (int i = std::max<int>(y, 0),
           size = std::min<int>(y + tile_type::SIZE, SIZE);
       i < size; i++) {
    data[i] |= (static_cast<tile_type::mask_type>(tile.data[i - y])
                << (BIT_MARGIN + x)) >>
               BIT_MARGIN;
  }
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (!tile.at(j, i)) continue;
      state[y + i][x + j] = tile.cell_value;
    }
  }
  mini = std::min<decltype(mini)>(mini, tile.cell_value);
  zk += tile.zk;
}

BitBoard::size_type BitBoard::blanks() const {
  return SIZE * SIZE - zk - initial_zk;
}
}
}
