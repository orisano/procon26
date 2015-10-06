#include "../Home.hpp"
#include "../BitBoard.hpp"
#include "../BitTile.hpp"
#include "../CacheTile.hpp"
#include "../Takumi/Util.hpp"
#include "../external/cmdline/cmdline.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

using namespace procon26;
using namespace std;

int main(int argc, char** argv) {
  cmdline::parser parser;
  parser.add<string>("filepath", 'f', "quest filepath", true);
  parser.parse_check(argc, argv);
  const string filepath = parser.get<string>("filepath");

  Home home;
  home.load(filepath);

  const int N = home.tiles.size();

  using tile_t = tile::BitTile;
  using tile_ex = CacheTile<tile_t>;
  using board_t = board::BitBoard;
  using board_ex = takumi::util::EBoard<board_t>;

  vector<tile_ex> tiles;
  for (int i = 0; i < N; i++) {
    tiles.emplace_back(tile_t(home.tiles[i]));
    tiles[i].fill(i);
  }
  board_ex board(board_t(home.board));

  while (1) {
    cout << "$)";
    string cmd;
    cin >> cmd;
    if (cmd == "d") {
      for (int i = 0; i < 32; i++) {
        std::printf("%02d", i);
      }
      std::puts("");
      takumi::util::dumpBoard(board);
    }
    if (cmd == "dd") takumi::util::dumpBoard(board, true);
    if (cmd == "check") {
      cout << "(id, rot, rev, x, y)\n";
      int id, rot, rev, x, y;
      cin >> id >> rot >> rev >> x >> y;
      tile_ex t = tiles[id];
      if (rev) t.reverse();
      while (rot--) t.rotate();

      if (board.canPut(t.value(), x, y)) {
        cout << "[" << cmd << "]"
             << "easy check: pass\n";
      }
      if (board.canPutStrong(t.value(), x, y)) {
        cout << "[" << cmd << "]"
             << "hard check: pass\n";
      }
    }
    if (cmd == "put") {
      cout << "(id, rot, rev, x, y)\n";
      int id, rot, rev, x, y;
      cin >> id >> rot >> rev >> x >> y;
      tile_ex t = tiles[id];
      if (rev) t.reverse();
      while (rot--) t.rotate();
      board.put(t.value(), x, y);
      takumi::util::dumpBoard(board, true);
    }
    if (cmd == "preview") {
      cout << "(id, rot, rev, x, y)\n";
      int id, rot, rev, x, y;
      cin >> id >> rot >> rev >> x >> y;
      tile_ex t = tiles[id];
      if (rev) t.reverse();
      while (rot--) t.rotate();
      auto b = board;
      b.put(t.value(), x, y);
      takumi::util::dumpBoard(b);
    }
    if (cmd == "display") {
      cout << "(id, rot, rev)\n";
      int id, rot, rev;
      cin >> id >> rot >> rev;
      tile_ex t = tiles[id];
      if (rev) t.reverse();
      while (rot--) t.rotate();
      takumi::util::dumpBoard(t.value());
    }
    if (cmd == "q") {
      break;
    }
  }
  return 0;
}
