#pragma once

#include <cstdint>

namespace procon26 {

template<typename T, int N>
struct Square {
    using cell_type = T;
    using size_type = std::uint16_t;
    using this_type = Square<T, N>;

    static const size_type SIZE = N;

    cell_type data[N][N];
    size_type zk;

    Square();

    void initialize();

    void copy(const this_type &square);

    void reverse();

    void rotate();

    void dump() const;

    inline size_type blanks() const;

    template<typename ConstIterator>
    ConstIterator load(ConstIterator iter);

    inline bool inBounds(int x, int y) const;

    inline cell_type at(int x, int y) const;
};

}

#include "details/Square.cpp"
