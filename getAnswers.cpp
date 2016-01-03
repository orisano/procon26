#include "Home.hpp"
#include "BitBoard.hpp"
#include <string>
#include <iostream>

using namespace std;
using namespace procon26;
using tile::Tile;

namespace {
inline bool ok(const procon26::board::BitBoard& b,
               const procon26::tile::Tile& tile, int x, int y) {
  for (int oy = 0; oy < 8; oy++) {
    for (int ox = 0; ox < 8; ox++) {
      int c = tile.at(ox, oy);
      if (!c) continue;
      if (!b.inBounds(x + ox, y + oy)) return false;
      if (b.at(x + ox, y + oy) != c) return false;
    }
  }
  return true;
}
}

void getAns(const Home& home, const board::BitBoard& board) {
  int tile_id = 2;
  auto tiles = home.tiles;
  for (auto& tile : tiles) {
    tile.fill(tile_id++);
    for (int y = -7; y < 31; y++) {
      for (int x = -7; x < 31; x++) {
        for (int inv = 0; inv < 2; inv++, tile.reverse()) {
          for (int rot = 0; rot < 4; rot++, tile.rotate()) {
            if (ok(board, tile, x, y)) {
              cout << x << " " << y << " "
                   << "HT"[inv] << " " << rot * 90 << "\r\n";
              goto NEXT;
            }
          }
        }
      }
    }
    cout << "\r\n";
  NEXT:
    ;
  }
}
