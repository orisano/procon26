#include "Home.hpp"
#include <string>
#include <iostream>

using namespace std;
using namespace procon26;
using tile::Tile;

inline bool inBounds(int x, int y){
  return 0 <= x && x < 32 && 0 <= y && y < 32;
}

inline bool ok(int b[32][32], Tile& tile, int x, int y){
  for (int oy = 0; oy < 8; oy++){
    for (int ox = 0; ox < 8; ox++){
      int c = tile.at(ox, oy);
      if (!c) continue;
      if (!inBounds(x + ox, y + oy)) return false;
      if (b[y + oy][x + ox] != c) return false;
    }
  }
  return true;
}

#define endl "\r\n"

int main(int argc, const char** argv)
{
  string filepath;
  if (argc > 1) filepath = string(argv[1]);
  else cout << "filepath:", cin >> filepath;

  Home home;
  home.load(filepath);

  int complete[32][32];
  for (int i = 0; i < 32; i++){
    for (int j = 0; j < 32; j++){
      cin >> complete[i][j];
    }
  }
  int tile_id = 2;
  for (auto& tile : home.tiles){
    tile.fill(tile_id++);
    for (int y = -7; y < 31; y++){
      for (int x = -7; x < 31; x++){
        for (int inv = 0; inv < 2; inv++, tile.reverse()){
          for (int rot = 0; rot < 4; rot++, tile.rotate()){
            if (ok(complete, tile, x, y)){
              cout << x << " " << y << " " << "HT"[inv] << " " << rot * 90 << endl;
              goto NEXT;
            }
          }
        }
      }
    }
    cout << endl;
NEXT:;
  }

  return 0;
}
