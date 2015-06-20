#pragma once
#include "Square.hpp"
#include "Tile.hpp"
#include <cstdint>

namespace procon26 {

struct Board : Square<short, 32> {
    typedef Square<short, 32> Derived;
    using size_type = std::uint16_t;

    Board();
    void put(const Tile& tile, int x, int y);
    bool puttable(const Tile& tile, int x, int y) const;
    size_type blanks() const;
  private:
    cell_type min_cell_;
};

}
