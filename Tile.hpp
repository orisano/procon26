#pragma once
#include "Square.hpp"
#include <cstdint>

namespace procon26 {
namespace tile {

struct Tile : Square<std::uint16_t, 8> {
    using Derived = Square<std::uint16_t, 8>;

    static const int BOARD_SIZE = 32;
    static const int OFFSET = 8;

    cell_type cell_value;
    cell_type mask[SIZE + 2][SIZE + 2];
    mutable bool cache[BOARD_SIZE + OFFSET][BOARD_SIZE + OFFSET];

    Tile();

    void fill(cell_type c);

    void rotate();

    void reverse();

    cell_type at(int x, int y) const;

    template<typename ConstIterator>
    ConstIterator load(ConstIterator iter) {
        auto result = Derived::load(iter);
        buildMask();
        return result;
    }

    void buildMask();

    bool canSkip(int x, int y) const;
    void setSkip(int x, int y) const;
};

}
}
