#pragma once

namespace procon26 {

template<typename T, int N>
struct Square {
  typedef T cell_type;
  typedef T this_type;
  static const int SIZE = N;

  cell_type data[N][N];

  Square() {}

  void initialize()
  {
    std::memset(data, 0, sizeof(data));
  }

  void copy(const this_type& square)
  {
    std::memcpy(data, square.data, sizeof(data));
  }

  /*
  O(N^2)
  事前に反転したピースを生成してO(1)でも可能、ただしメモリ使用量が2倍になる
  */
  void reverse()
  {
    cell_type reverse_buffer[N][N];
    for (auto y = 0; y < N; ++y) {
      for (auto x = 0; x < N; ++x) {
        reverse_buffer[y][x] = data[y][N - x - 1];
      }
    }
    std::memcpy(data, reverse_buffer, sizeof(data));
  }

  /*
  O(N^2)
  事前に回転したピースを生成してO(1)でも可能、ただしメモリ使用量が4倍になる
  */
  void rotate()
  {
    cell_type rotate_buffer[N][N];
    for (auto y = 0; y < N; ++y) {
      for (auto x = 0; x < N; ++x) {
        rotate_buffer[y][x] = data[N - x - 1][y];
      }
    }
    std::memcpy(data, rotate_buffer, sizeof(data));
  }

  void fill(cell_type c)
  {
    for (auto y = 0; y < N; ++y) {
      for (auto x = 0; x < N; ++x) {
        if (!data[y][x]) continue;
        data[y][x] = c;
      }
    }
  }

  template<typename ConstIterator>
  ConstIterator load(ConstIterator iter)
  {
    for (auto y = 0; y < N; ++y) {
      for (auto x = 0; x < N; ++x) {
        data[y][x] = ((*iter)[x] == '1');
      }
      ++iter;
    }
    return iter;
  }

  cell_type* operator[](int index)
  {
    return data[index];
  }

  cell_type at(int x, int y) const
  {
    return data[y][x];
  }
};

typedef Square<int, 8> Tile;

struct Board : Square<int, 32> {
  typedef Square<int, 32> Derived;

  void put(const Tile& tile, int x, int y)
  {
    assert(puttable(tile, x, y));
    for (auto yi = 0; yi < Tile::SIZE; ++yi) {
      for (auto xi = 0; xi < Tile::SIZE; ++xi) {
        if (tile.data[yi][xi] == 0) continue;
        const auto nx = x + xi, ny = y + yi;
        data[ny][nx] = tile.data[yi][xi];
      }
    }
  }

  bool puttable(const Tile& tile, int x, int y) const
  {
    for (auto yi = 0; yi < Tile::SIZE; ++yi) {
      for (auto xi = 0; xi < Tile::SIZE; ++xi) {
        if (tile.data[yi][xi] == 0) continue;
        const auto nx = x + xi, ny = y + yi;
        if (nx < 0 || SIZE <= nx || ny < 0 || SIZE <= ny) return false;
        if (data[ny][nx] != 0) return false;
      }
    }
    return true;
  }
};

}
