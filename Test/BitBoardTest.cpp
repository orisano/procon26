#include "../BitBoard.hpp"
#include "../BitTile.hpp"
#include "../CacheTile.hpp"
#include "../external/cmdline/cmdline.h"
#include <cassert>
#include <cstdio>
#include <vector>
#include <string>

namespace {

template <typename T>
inline void dumpBin(T x) {
  for (int i = 0; i < sizeof(T) * 8; i++) {
    std::printf("%d", (x >> i) & 1);
  }
  std::printf("\n");
}

template <typename T>
inline void dumpData(const T& data) {
  std::puts("---------------------------------");
  for (int y = 0; y < T::SIZE; y++) {
    for (int x = 0; x < T::SIZE; x++) {
      std::printf("%3d", data.at(x, y));
    }
    std::puts("");
  }

  std::puts("---------------------------------");
}
}

using namespace std;
using namespace procon26;
using namespace procon26::tile;
using namespace procon26::board;

int main(int argc, const char** argv) {
  vector<string> data_board{
      "11001001000000000000000000000000", "11000000000000000000000000000000",
      "11001001000000000000000000000000", "00000000000000000000000000000000",
      "00000000000000000000000000000000", "00000100000000000000000000000000",
      "00000000000000000000000000000000", "00000000000000000000000000000000",
      "00000001000000000000000000000000", "00000000000000000000000000000000",
      "00000000000000000000000000000000", "00000000000000000000000000000000",
      "00000000000000000000000000000000", "00000010000000000000000000000000",
      "00000000000000000000000000000000", "00000000000000000000000000000000",
      "00000000000000000000000000000000", "00000000000000000000000000000000",
      "00000000000000000000000000000000", "00000000000000000000000000000000",
      "00000000000000000000000000000000", "00000000000000000000000000000000",
      "00000000000000000000000000000000", "00000000000000000000000000000000",
      "00000000000000000000000000000000", "00000000000000000000000000000000",
      "00000000000000000000000000000000", "00000000000000000000000000000000",
      "00000000000000000000000000000000", "00000000000000000000000000000000",
      "00000000000000000000000000000000", "00000000000000000000000000000000", };
  Board raw_board;
  raw_board.load(data_board.begin());

  vector<string> data_tile_1{"11100000", "01000000", "01100000", "00000000",
                             "00000000", "00000000", "00000000", "00000000", };
  vector<string> data_tile_2{"01000000", "11110000", "01000000", "00000000",
                             "00000000", "00000000", "00000000", "00000000", };
  Tile raw_tile_1;
  raw_tile_1.load(data_tile_1.begin());
  Tile raw_tile_2;
  raw_tile_2.load(data_tile_2.begin());

  BitBoard board(raw_board);
  BitTile btile_1(raw_tile_1);
  BitTile btile_2(raw_tile_2);
  BitTile btile_3(raw_tile_1);
  BitTile btile_4(raw_tile_2);

  CacheTile<BitTile> tile_1(btile_1);
  CacheTile<BitTile> tile_2(btile_2);
  CacheTile<BitTile> tile_3(btile_3);
  CacheTile<BitTile> tile_4(btile_4);

  tile_1.fill(0);
  tile_2.fill(1);
  tile_3.fill(2);
  tile_3.reverse();
  tile_4.fill(3);
  tile_4.rotate();

  if (board.canPutStrong(tile_1.value(), 0, 3)) board.put(tile_1.value(), 0, 3);
  if (board.canPutStrong(tile_2.value(), 2, 3)) board.put(tile_2.value(), 2, 3);
  if (board.canPutStrong(tile_4.value(), 0, 2)) board.put(tile_4.value(), 0, 2);

  dumpData(board);
  dumpData(tile_1.value());
  dumpData(tile_2.value());
  dumpData(tile_3.value());
  dumpData(tile_4.value());

  return 0;
}
