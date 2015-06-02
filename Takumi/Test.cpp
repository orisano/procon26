#include "Test.hpp"

namespace {
const int dx[] = {0, 0, 1, -1};
const int dy[] = {1, -1, 0, 0};
const int SIZE = 32;
const int BEAM_WIDTH = 100;
const int ONE_STEP = 200;

inline int inBounds(int x, int y){
  return 0 <= x && x < SIZE && 0 <= y && y < SIZE;
}

int evalBoard(Board& board)
{
  int maxi = 0;
  int blank_count = 0;
  for (int y = 0; y < SIZE; y++){
    for (int x = 0; x < SIZE; x++){
      int c = board.data[y][x];
      if (maxi < c) maxi = c;
      if (c == 0) blank_count++;
    }
  }

  int puttable_count = 0;
  if (maxi > 1){
    int vis[SIZE][SIZE] = {};
    for (int y = 0; y < SIZE; y++){
      for (int x = 0; x < SIZE; x++){
        if (board.data[y][x] < 1) continue;
        for (int d = 0; d < 4; d++){
          int nx = x + dx[d], ny = y + dy[d];
          if (!inBounds(nx, ny)) continue;
          if (board.data[ny][nx] != 0) continue;
          puttable_count += !vis[ny][nx]++;
        }
      }
    }
  }
  else puttable_count = blank_count;

  return 4 * blank_count - (puttable_count * puttable_count);
}
}

namespace procon26 {
namespace takumi {

Answer Test::solve(const Home& home, int millsec)
{
  Board initial = home.board;
  std::vector<Tile> tiles = home.tiles;

  vector<Board> beam;
  beam.emplace_back(initial);
  /*
   * あとで重複削除入れる
   * boardの状態と使ったピースのidでzobristhashで殴る
   */
  auto start = std::chrono::high_resolution_clock::now();
  while (std::chrono::duration_cast<std::chrono::millseconds>(std::chrono::high_resolution_clock::now() - start).count() + ONE_STEP < millsec){
  }
}

}
}
