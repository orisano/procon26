#pragma once

#include "../Board.hpp"
#include "../BitBoard.hpp"

namespace procon26 {
namespace eval {

namespace {
const int dx[] = {0, 0, 1, -1, 1, 1, -1, -1};
const int dy[] = {1, -1, 0, 0, 1, -1, 1, -1};
const int SIZE = 32;
}

using Board    = procon26::board::Board;
using BitBoard = procon26::board::BitBoard;

inline int A(const BitBoard& board)
{
  static const int pn[] = {0, 1, 3, 7, 12};
  
  int density = 0;
  for (int y = 0; y < SIZE; y++){
    for (int x = 0; x < SIZE; x++){
      if (board.at(x, y)) continue;
      int cnt = 0;
      for (int d = 0; d < 4; d++){
        const int nx = x + dx[d], ny = y + dy[d];
        if (!board.inBounds(nx, ny)) cnt++;
        else if (board.at(nx, ny) != 0) cnt++;
      }
      density += pn[cnt];
    }
  }

  return board.blanks() + density;
}

inline int B(const Board& board)
{
  static const int pn[] = {0, 1, 1, 1, 1, 1, 1, 1, 1};
  int maxi = 0;
  for (int y = 0; y < SIZE; y++){
    for (int x = 0; x < SIZE; x++){
      int c = board.data[y][x];
      if (maxi < c) maxi = c;
    }
  }
  int density = 0;
  for (int y = 0; y < SIZE; y++){
    for (int x = 0; x < SIZE; x++){
      if (board.data[y][x] != 0) continue;
      int cnt = 0;
      for (int d = 0; d < 8; d++){
        const int nx = x + dx[d], ny = y + dy[d];
        if (!board.inBounds(nx, ny)) cnt++;
        else if (board.data[ny][nx] != 0) cnt++;
      }
      density += pn[cnt];
    }
  }
  return board.blanks() + density + maxi;
}

inline int C(const Board& board)
{
  static const int pn[] = {0, 1, 3, 7, 10, 14, 18, 22, 26};
  int maxi = 0;
  for (int y = 0; y < SIZE; y++){
    for (int x = 0; x < SIZE; x++){
      int c = board.data[y][x];
      if (maxi < c) maxi = c;
    }
  }
  int density = 0;
  for (int y = 0; y < SIZE; y++){
    for (int x = 0; x < SIZE; x++){
      if (board.data[y][x] != 0) continue;
      int cnt = 0;
      for (int d = 0; d < 8; d++){
        const int nx = x + dx[d], ny = y + dy[d];
        if (!board.inBounds(nx, ny)) cnt++;
        else if (board.data[ny][nx] != 0) cnt++;
      }
      density += pn[cnt];
    }
  }
  return board.blanks() + density + maxi;
}

inline int D(const Board& board)
{
  static const int pn[] = {0, 1, 3, 7, 10, 14, 18, 22, 26};
  int maxi = 0;
  for (int y = 0; y < SIZE; y++){
    for (int x = 0; x < SIZE; x++){
      int c = board.data[y][x];
      if (maxi < c) maxi = c;
    }
  }
  int density = 0;
  for (int y = 0; y < SIZE; y++){
    for (int x = 0; x < SIZE; x++){
      if (board.data[y][x] != 0) continue;
      int cnt = 0;
      for (int d = 0; d < 4; d++){
        const int nx = x + dx[d], ny = y + dy[d];
        if (!board.inBounds(nx, ny)) cnt++;
        else if (board.data[ny][nx] != 0) cnt++;
      }
      density += pn[cnt];
    }
  }
  return board.blanks() + density + maxi;
}

} // namespace eval
} // namespace procon26
