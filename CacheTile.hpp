#pragma once
#include "Tile.hpp"
#include <array>

namespace procon26 {

template <typename TileType>
struct CacheTile {
  std::array<TileType, 8> cache;

  CacheTile(const TileType& tile) : rotate_(0), inverse_(0) {
    cache[0] = tile;
    cache[4] = tile;
    cache[4].reverse();
    for (int i = 1; i < 4; i++) {
      cache[i] = cache[i - 1];
      cache[i].rotate();
      cache[i + 4] = cache[i + 3];
      cache[i + 4].rotate();
    }
  }
  void fill(int c) {
    for (int i = 0; i < 8; i++) {
      cache[i].fill(c);
    }
  }
  void rotate() { rotate_ = (rotate_ + 1) % 4; }
  void reverse() { inverse_ = !inverse_; }
  inline int at(int x, int y) const {
    return cache[rotate_ * 2 + inverse_].at(x, y);
  }
  const TileType& value() { return cache[rotate_ * 2 + inverse_]; }
  const std::array<TileType, 8>& iter() { return cache; }

 private:
  int rotate_;
  int inverse_;
};
}
