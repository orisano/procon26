#include "BitBeam.hpp"
#include <algorithm>
#include <cstdio>
#include <chrono>
#include <set>
#include <unordered_set>
#include "../BitBoard.hpp"
#include "../BitTile.hpp"
#include "../CacheTile.hpp"
#include "ZobristHash.hpp"
#include "../../../../orliv/benchmark.hpp"
#include "../../../../orliv/uf.hpp"
#include "../../../../orliv/bool_array.hpp"
#include "../external/cmdline/cmdline.h"

// #define BEAM_BENCH
// #define EVAL_ERASE
#define SHOW_BEST

namespace {
const std::array<int, 24> dx = {0, 0, 1,  -1, 1, 1,  -1, -1, 2, -2, 0, 0,
                                2, 2, -2, -2, 1, -1, 1,  -1, 3, -3, 0, 0};
const std::array<int, 24> dy = {1, -1, 0, 0,  1, -1, -1, 1,  0, 0, 2, -2,
                                1, -1, 1, -1, 2, 2,  -2, -2, 0, 0, 3, -3};
const int SIZE = 32;
const int pn[] = {0, 1, 3, 7, 20};
// const int pn[] = {0, 1, 5, 20, 20};
// const int pn[] = {0, 1, 3, 7, 12};

using procon26::board::BitBoard;

int getArea(const BitBoard &board) {
  orliv::UF2d uf(SIZE, SIZE);
  for (int y = 0; y < SIZE; y++) {
    for (int x = 0; x < SIZE - 1; x++) {
      auto v = board.at(x, y);
      auto nv = board.at(x + 1, y);
      if ((v != 0) == (nv != 0)) uf.merge(x, y, x + 1, y);
    }
  }
  for (int y = 0; y < SIZE - 1; y++) {
    for (int x = 0; x < SIZE; x++) {
      auto v = board.at(x, y);
      auto nv = board.at(x, y + 1);
      if ((v != 0) == (nv != 0)) uf.merge(x, y, x, y + 1);
    }
  }

  int res = 0;

  static orliv::BoolArray<2048> ba;
  ba.clear();
  for (int y = 0; y < SIZE; y++) {
    for (int x = 0; x < SIZE; x++) {
      if (board.at(x, y) == 0) {
        auto p = uf.root(x, y);
        if (!ba.get(p)) {
          if (uf.size(x, y) <= 4) {
            res++;
          }
          ba.set(p);
        }
      }
    }
  }
  return res;
}

inline int evalBoard(const BitBoard &board) {
  /*
    static orliv::BoolArray<2048> ba;
    ba.clear();
    int cnt = 0;
    for (int y = 0; y < SIZE; y++){
      for (int x = 0; x < SIZE; x++){
        if (board.at(x, y) < 2) continue;
        for (int d = 0; d < dx.size(); d++){
          const int nx = x + dx[d], ny = y + dy[d];
          if (!board.inBounds(nx, ny)) continue;
          const int nv = ny * SIZE + nx;
          if (ba.get(nv)) continue;
          cnt += !board.at(nx, ny);
          ba.set(nv);
        }
      }
    }
    return -cnt + board.blanks();
  */
  int density = 0;
  for (int y = 0; y < SIZE; y++) {
    for (int x = 0; x < SIZE; x++) {
      if (board.at(x, y)) continue;
      int cnt = 0;
      for (int d = 0; d < 4; d++) {
        int nx = x + dx[d], ny = y + dy[d];
        if (!board.inBounds(nx, ny)) {
          cnt++;
          continue;
        }
        cnt += board.at(nx, ny) != 0;
      }
      density += pn[cnt];
    }
  }
  int area = 0;
  area = getArea(board);
  return board.blanks() + density + 50 * area;
}

int getColor(int c) {
  if (c == 0) return 47;
  if (c == 1) return 40;
  return (c - 2) % 6 + 41;
}

void dumpBoard(const BitBoard &board, bool number = false) {
  puts("----------------");
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 32; j++) {
      auto c = board.at(j, i);
      if (number)
        printf("%4d", c);
      else
        printf("\x1b[%dm  \x1b[49m", getColor(c));
    }
    puts("");
  }
  printf("blanks: %d\n", board.blanks());
  puts("----------------");
}

struct EBoard : public BitBoard {
  using Derived = BitBoard;

  EBoard() : Derived(), eval(-1) {
    used_[0] = used_[1] = used_[2] = used_[3] = 0;
  }

  EBoard(Derived b) : Derived(b), eval(-1) {
    used_[0] = used_[1] = used_[2] = used_[3] = 0;
  }

  bool isUsed(int id) const {
    assert(0 <= id && id < 256);
    return ((used_[id >> 6] >> (id & 63)) & 1) != 0;
  }

  void useTile(int id) {
    eval = -1;
    assert(!isUsed(id));
    used_[id >> 6] |= 1ull << (id & 63);
  }

  int eval;

 private:
  std::uint64_t used_[4];
};
}

namespace procon26 {
namespace takumi {

Answer BitBeam::solve(const Home &home, int millisec, cmdline::parser &parser) {
  using tile::BitTile;

  const int BEAM_WIDTH = parser.get<int>("beam_width");

  BitBoard initial(home.board);
  std::vector<CacheTile<BitTile>> tiles;
  int id = 0;
  for (auto &tile : home.tiles)
    tiles.emplace_back(BitTile(tile)), tiles.back().fill(id++);
  std::vector<EBoard> beam;
  beam.emplace_back(initial);
  orliv::ZobristHash<std::uint64_t, 32, 32, 2> zb;

  // auto start = std::chrono::high_resolution_clock::now();
  std::vector<EBoard> nxt;
  nxt.reserve(550000);
  std::unordered_set<std::uint64_t> vis;
  vis.reserve(550000);

  auto best = initial;
  int tile_id = 0;
  while ((
      tile_id < (int)tiles.size() &&
      beam.size())) {  // std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()
                       // - start).count() + ONE_STEP < millisec) {
    printf("tile_id: %d / %lu\n", tile_id, tiles.size());
    // dumpBoard(beam[0]);
    nxt.clear();
    vis.clear();
#ifdef SHOW_BEST
    dumpBoard(best);
#else
    puts("view");
    for (const auto &b : beam) dumpBoard(b), getchar();
#endif
    benchmark("next beam generate") {
      for (const auto &b : beam) {
        nxt.emplace_back(b);
#ifdef BEAM_BENCH
        benchmark("beam") {
#endif
          auto &tile = tiles[tile_id];
          for (int y = -7; y < 32; y++) {
            for (int x = -7; x < 32; x++) {
              for (int r = 0; r < 4; r++, tile.rotate()) {
                if (!b.canPut(tile.value(), x, y)) continue;
                EBoard nb = b;
                nb.put(tile.value(), x, y);
                nb.useTile(tile_id);
                if (vis.count(nb.hashv)) continue;
                vis.insert(nb.hashv);
                nxt.emplace_back(std::move(nb));
              }
              tile.reverse();
              for (int r = 0; r < 4; r++, tile.rotate()) {
                if (!b.canPut(tile.value(), x, y)) continue;
                EBoard nb = b;
                nb.put(tile.value(), x, y);
                nb.useTile(tile_id);
                if (vis.count(nb.hashv)) continue;
                vis.insert(nb.hashv);
                nxt.emplace_back(std::move(nb));
              }
            }
          }
#ifdef BEAM_BENCH
        }
#endif
      }
    }
    auto bbest = std::min_element(nxt.begin(), nxt.end(),
                                  [](const EBoard &a, const EBoard &b) {
      return a.blanks() < b.blanks();
    });
    if (bbest != nxt.end() && bbest->blanks() < best.blanks()) {
      best = *bbest;
    }
    printf("nxt size: %lu\n", nxt.size());
    if ((int)nxt.size() > BEAM_WIDTH) {
#ifndef EVAL_ERASE
      benchmark("eval") {
        std::for_each(nxt.begin(), nxt.end(), [](EBoard &b) {
          if (b.eval == -1) b.eval = evalBoard(b);
        });
      }
#endif
      benchmark("sort") {
        std::partial_sort(
            begin(nxt), begin(nxt) + BEAM_WIDTH, end(nxt),
            [](const EBoard &a, const EBoard &b) { return a.eval < b.eval; });
      }
      nxt.erase(begin(nxt) + BEAM_WIDTH, end(nxt));
    }
    std::swap(beam, nxt);
    tile_id++;
  }
  dumpBoard(best, true);
  return Answer();
}

cmdline::parser BitBeam::getParser() {
  cmdline::parser parser;
  parser.add<int>("beam_width", 'b', "beam width", false, 1000);

  return parser;
}
}
}
