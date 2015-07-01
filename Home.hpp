#pragma once
#include "Square.hpp"
#include "Action.hpp"
#include "Board.hpp"
#include "Tile.hpp"
#include <vector>

namespace procon26 {

struct Home {
  board::Board board;
  std::vector<tile::Tile> tiles;
  void load(const std::string& filepath);
  tile::Tile toTile(Action action) const;
};

}

