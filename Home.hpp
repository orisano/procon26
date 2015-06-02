#pragma once
#include "Square.hpp"
#include <vector>

namespace procon26 {

template<typename BoardType, typename TileType>
struct HomeImpl {
  BoardType board;
  std::vector<TileType> tiles;
  void load(const std::string& filepath);
};

typedef HomeImpl<Board, Tile> Home;
typedef HomeImpl<GBoard, GTile> GHome;

}

#include "details/Home.cpp"