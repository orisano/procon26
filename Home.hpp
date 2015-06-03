#pragma once
#include "Square.hpp"
#include "Action.hpp"
#include "Board.hpp"
#include "Tile.hpp"
#include <vector>

namespace procon26 {

template<typename BoardType, typename TileType>
struct HomeImpl {
  BoardType board;
  std::vector<TileType> tiles;
  void load(const std::string& filepath);
  TileType toTile(Action action) const;
};

typedef HomeImpl<Board, Tile> Home;

}

#include "details/Home.cpp"
