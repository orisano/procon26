#pragma once
#include "Tile.hpp"

namespace procon26 {

template<typename TileType>
struct CacheTile {
  TileType cache[4][2];

  CacheTile(const TileType& tile) : rotate_(0), inverse_(0) {
    cache[0][0] = tile;
    cache[0][1] = tile;
    cache[0][1].reverse();
    for (int i = 1; i < 4; i++){
      for (int j = 0; j < 2; j++){
        cache[i][j] = cache[i - 1][j];
        cache[i][j].rotate();
      }
    }
  }
  void fill(int c){
    for (int i = 0; i < 4; i++){
      for (int j = 0; j < 2; j++){
        cache[i][j].fill(c);
      }
    }
  }
  void rotate() {
    rotate_ = (rotate_ + 1) % 4;
  }
  void reverse() {
    inverse_ = !inverse_;
  }
  inline int at(int x, int y) const {
    return cache[rotate_][inverse_].at(x, y);
  }
  const TileType& value(){
    return cache[rotate_][inverse_];
  }
private:
  int rotate_;
  int inverse_;
};

}
