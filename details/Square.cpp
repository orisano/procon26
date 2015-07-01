#include "../Square.hpp"
#include <algorithm>
#include <cassert>
#include <cstring>

namespace procon26 {

template<typename T, int N>
Square<T, N>::Square() { }

template<typename T, int N>
void Square<T, N>::initialize() {
    std::memset(data, 0, sizeof(data));
}

template<typename T, int N>
void Square<T, N>::copy(const this_type &square) {
    std::memcpy(data, square.data, sizeof(data));
}

template<typename T, int N>
void Square<T, N>::reverse() {
    cell_type reverse_buffer[N][N];
    for (auto y = 0; y < N; ++y) {
        for (auto x = 0; x < N; ++x) {
            reverse_buffer[y][x] = data[y][N - x - 1];
        }
    }
    std::memcpy(data, reverse_buffer, sizeof(data));
}

template<typename T, int N>
void Square<T, N>::rotate() {
    cell_type rotate_buffer[N][N];
    for (auto y = 0; y < N; ++y) {
        for (auto x = 0; x < N; ++x) {
            rotate_buffer[y][x] = data[N - x - 1][y];
        }
    }
    std::memcpy(data, rotate_buffer, sizeof(data));
}

template<typename T, int N>
inline typename Square<T, N>::size_type Square<T, N>::blanks() const {
    return N * N - zk;
}

template<typename T, int N>
inline bool Square<T, N>::inBounds(int x, int y) const {
    return 0 <= x && x < N && 0 <= y && y < N;
}

template<typename T, int N>
typename Square<T, N>::cell_type Square<T, N>::at(int x, int y) const {
    assert(inBounds(x, y));
    return data[y][x];
}

template<typename T, int N>
template<typename ConstIterator>
ConstIterator Square<T, N>::load(ConstIterator iter) {
    zk = 0;
    for (auto y = 0; y < N; ++y) {
        for (auto x = 0; x < N; ++x) {
            data[y][x] = ((*iter)[x] == '1');
            zk += data[y][x];
        }
        ++iter;
    }
    return iter;
}
}
