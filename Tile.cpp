#include "Tile.hpp"
#include <cassert>
#include <cstring>
#include <algorithm>

namespace {

template<typename T>
inline void update(T& x, T y){ x = (x ? std::min(x, y) : y); }

}

namespace procon26 {
namespace tile {

Tile::Tile() : Derived(), cell_value(1) { }

void Tile::fill(cell_type c) {
    cell_value = c;
}

Tile::Derived::cell_type Tile::at(int x, int y) const {
    assert(inBounds(x, y));
    return data[y][x] * cell_value;
}

void Tile::buildMask() {
    std::memset(mask, 0, sizeof(mask));
    std::memset(cache, 0, sizeof(cache));

    for (int y = 0; y < SIZE; y++){
        for (int x = 0; x < SIZE; x++){
            if (data[y][x] == 0) continue;
            update(mask[y + 1][x + 0], data[y][x]);
            update(mask[y + 1][x + 2], data[y][x]);
            update(mask[y + 0][x + 1], data[y][x]);
            update(mask[y + 2][x + 1], data[y][x]);
        }
    }
}

void Tile::rotate() {
    Derived::rotate();
    buildMask();
}

void Tile::reverse() {
    Derived::reverse();
    buildMask();
}

bool Tile::canSkip(int x, int y) const {
    return cache[OFFSET + y][OFFSET + x];
}

void Tile::setSkip(int x, int y) const {
    cache[OFFSET + y][OFFSET + x] = true;
}

}
}
