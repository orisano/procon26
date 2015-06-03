#pragma once

namespace square_internal {
const int dx[] = {1, -1, 0, 0};
const int dy[] = {0, 0, -1, 1};
}

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

  inline bool inBounds(int x, int y) const
  {
    return 0 <= x && x < N && 0 <= y && y < N;
  }

  cell_type* operator[](int index)
  {
    assert(0 <= index && index < N);
    return data[index];
  }

  cell_type at(int x, int y) const
  {
    assert(inBounds(x, y));
    return data[y][x];
  }
};

struct Tile : Square<int, 8> {
  typedef Square<int, 8> Derived;

  Tile() : cell_value(1) {}

  void fill(cell_type c)
  {
    cell_value = c;
  }

  cell_type at(int x, int y) const
  {
    assert(inBounds(x, y));
    return data[y][x] * cell_value;
  }

  cell_type cell_value;
};

struct Board : Square<int, 32> {
  typedef Square<int, 32> Derived;

  Board() : min_cell_(512) {}

  void put(const Tile& tile, int x, int y)
  {
    assert(puttable(tile, x, y));
    auto cv = tile.cell_value;
    for (auto yi = 0; yi < Tile::SIZE; ++yi) {
      for (auto xi = 0; xi < Tile::SIZE; ++xi) {
        if (tile.data[yi][xi] == 0) continue;
        const auto nx = x + xi, ny = y + yi;
        data[ny][nx] = cv;
      }
    }
    min_cell_ = std::min(min_cell_, cv);
  }

  bool puttable(const Tile& tile, int x, int y) const
  {
    auto min_cv = 511;
    for (auto yi = 0; yi < Tile::SIZE; ++yi) {
      for (auto xi = 0; xi < Tile::SIZE; ++xi) {
        if (tile.data[yi][xi] == 0) continue;
        const auto cx = x + xi, cy = y + yi;
        if (!inBounds(cx, cy)) return false;
        if (data[cy][cx] != 0) return false;
        for (auto d = 0; d < 4; d++) {
          const auto nx = cx + square_internal::dx[d], ny = cy + square_internal::dy[d];
          if (!inBounds(nx, ny)) continue;
          const auto c = data[ny][nx];
          if (c <= 1) continue;
          min_cv = std::min(min_cv, c);
        }
      }
    }
    return min_cell_ == 512 || min_cv == min_cell_ || min_cv < tile.cell_value;
  }

private:
  cell_type min_cell_;
};

}
