#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <cstdint>
#include <tuple>

#include "Tile.hpp"

namespace {

struct Point {
    int x, y;

    Point(int x, int y) : x(x), y(y) {}

    bool operator<(const Point& rhs) const {
        return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
};

}

namespace procon26 {
namespace tile {

struct LinkedTile {
    using cell_type = std::uint16_t;
    using size_type = std::uint16_t;

    static const int SIZE = 8;
    std::vector<Point> data;
    size_type zk;
    cell_type cell_value;

    LinkedTile(const Tile &tile);
    void reverse();
    void rotate();
    size_type blanks() const;
    inline cell_type at(int x, int y) const;
};

}
}