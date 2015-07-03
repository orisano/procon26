#include "BitBoard.hpp"

namespace {
const int BIT_MARGIN = 10;
}

namespace procon26 {
namespace board {

using procon26::tile::BitTile;

BitBoard::cell_type BitBoard::initial[BitBoard::SIZE];
BitBoard::size_type BitBoard::initial_zk;

BitBoard::BitBoard() {
    std::memset(initial, 0, sizeof(initial));
    initial_zk = 0;
    std::memset(data, 0, sizeof(data));
    std::memset(state, 0, sizeof(state));
    zk = 0;
}

BitBoard::BitBoard(const Board &board) {
    std::memset(initial, 0, sizeof(initial));
    initial_zk = 0;
    std::memset(data, 0, sizeof(data));
    std::memset(state, 0, sizeof(state));
    zk = 0;

    const cell_type ONE = 1;
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            auto c = board.data[y][x];
            if (!c) continue;
            if (c == 1) initial[y] |= ONE << x, initial_zk++;
            else data[y] |= ONE << x, zk++;
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

bool BitBoard::canPut(const BitTile &tile, int x, int y) const {
    using mask_type = typename BitTile::mask_type;
    const mask_type zero = 0;

    if (tile.zk > blanks()) return false;

    mask_type conflict_mask[BitTile::SIZE];
    for (int i = 0; i < BitTile::SIZE; i++) {
        conflict_mask[i] = ~((inBounds(0, y + i) ? ~(data[y + i] | initial[y + i]) : zero) << BIT_MARGIN);
    }

    mask_type conflict = 0;
    for (int i = 0; i < BitTile::SIZE; i++) {
        conflict |= conflict_mask[i] & (static_cast<mask_type>(tile.data[i]) << (BIT_MARGIN + x));
    }
    if (conflict) return false;

    if (!zk) return true;

    mask_type neighbor_mask[BitTile::MASK_SIZE];
    for (int i = 0; i < BitTile::MASK_SIZE; i++) {
        neighbor_mask[i] = (inBounds(0, y + i - 1) ? data[y + i - 1] : zero) << BIT_MARGIN;
    }

    mask_type neighbor = 0;
    for (int i = 0; i < BitTile::MASK_SIZE; i++) {
        neighbor |= neighbor_mask[i] & (tile.mask[i] << (BIT_MARGIN + x - 1));
    }
    return neighbor != zero;
}

void BitBoard::put(const BitTile &tile, int x, int y) {
    assert(canPut(tile, x, y));
    for (int i = std::max<int>(y, 0), size = std::min<int>(y + BitTile::SIZE, SIZE); i < size; i++) {
        data[i] |= (static_cast<BitTile::mask_type>(tile.data[i - y]) << (BIT_MARGIN + x)) >> BIT_MARGIN;
    }
    for (int i = 0; i < 8; i++){
      for (int j = 0; j < 8; j++){
        if (!tile.at(j, i)) continue;
        state[y + i][x + j] = tile.cell_value;
      }
    }
    zk += tile.zk;
}

BitBoard::size_type BitBoard::blanks() const {
    return SIZE * SIZE - zk - initial_zk;
}

}
}
