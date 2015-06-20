#include "BitTile.hpp"
#include <cassert>
#include <cstring>

namespace procon26 {

BitTile::BitTile() {

}

BitTile::BitTile(const Tile &tile) {
    std::memset(data, 0, sizeof(data));
    zk = 0;
    for (int y = 0; y < SIZE; y++){
        for (int x = 0; x < SIZE; x++){
            if (!tile.at(x, y)) continue;
            data[y] |= static_cast<cell_type>(1) << x;
            ++zk;
        }
    }
    buildMask();
}

bool BitTile::inBounds(int x, int y) const {
    return 0 <= x && x < SIZE && 0 <= y && y < SIZE;
}

void BitTile::buildMask() {
    std::memset(mask, 0, sizeof(mask));
    for (int y = 0; y < SIZE; y++){
        mask[y]     |= data[y] << 1;
        mask[y + 1] |= data[y];
        mask[y + 1] |= data[y] << 2;
        mask[y + 2] |= data[y] << 1;
    }
}

void BitTile::reverse() {
    cell_type reverse_buffer[SIZE] = {};
    for (int y = 0; y < SIZE; y++){
        for (int x = 0; x < SIZE; x++){
            if (!at(x, y)) continue;
            reverse_buffer[y] |= static_cast<cell_type>(1) << (SIZE - x - 1);
        }
    }
    std::memcpy(data, reverse_buffer, sizeof(data));
    buildMask();
}

void BitTile::rotate() {
    cell_type rotate_buffer[SIZE] = {};
    for (int y = 0; y < SIZE; y++){
        for (int x = 0; x < SIZE; x++){
            if (!at(x, y)) continue;
            rotate_buffer[x] |= static_cast<cell_type>(1) << (SIZE - y - 1);
        }
    }
    std::memcpy(data, rotate_buffer, sizeof(data));
    buildMask();
}

BitTile::cell_type BitTile::at(int x, int y) const {
    assert(inBounds(x, y));
    return (data[y] >> x) & 1;
}

void BitTile::fill(int c) {

}

}