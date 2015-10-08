#include "Beam.hpp"
#include "Util.hpp"
#include <algorithm>
#include <cstdio>
#include <chrono>
#include <set>
#include <unordered_set>
#include "../BitBoard.hpp"
#include "../Tile.hpp"
#include "../CacheTile.hpp"
#include "ZobristHash.hpp"
#include "../external/orliv/benchmark.hpp"
#include "../external/cmdline/cmdline.h"

// #define BEAM_BENCH

namespace {
const int dx[] = {0, 0, 1, -1, 1, 1, -1, -1};
const int dy[] = {1, -1, 0, 0, 1, -1, 1, -1};
const int ONE_STEP = 200;
const int SIZE = 32;
const int SMALL_TILE_PENALTY = 20;
// const int tri[] = {0, 1, 4, 10, 12};
const int pn[] = {0, 1, 3, 7, 10, 14, 18, 22, 26};
// const int pn[] = {0, 1, 15, 7, 20, 14, 18, 22, 26};
// const int pn[] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

struct Evaluater {
  using Home = procon26::Home;

  Evaluater(const Home &home) : home(home), N(home.tiles.size()) {}

  struct Functor {
    const Evaluater *const that;
    Functor(const Evaluater *const that) : that(that) {}
    template <typename T>
    void operator()(T &board) {
      board.eval = that->eval(board);
    }
  };

  template <typename T>
  inline int eval(const T &board) const {
    int density = 0;
    for (int y = 0; y < SIZE; y++) {
      for (int x = 0; x < SIZE; x++) {
        int c = board.at(x, y);
        if (c != 0) continue;
        int cnt = 0;
        for (int d = 0; d < 4; d++) {
          int nx = x + dx[d], ny = y + dy[d];
          if (!board.inBounds(nx, ny)) {
            cnt++;
            continue;
          }
          int c = board.at(nx, ny);
          cnt += (c != 0);
        }
        density += pn[cnt];
      }
    }
    int penalty = 0;
    for (int i = 0; i < N; i++) {
      if (board.isUsed(i) && home.tiles[i].zk <= 4u) {
        penalty += SMALL_TILE_PENALTY + i * 3;
      }
    }
    return board.blanks() + density + penalty + board.maxi;
  }
  Functor &&operator()() { return std::move(Functor(this)); }
  const Home home;
  const int N;
};
}
namespace procon26 {
namespace takumi {

Answer Beam::solve(const Home &home, const int millisec,
                   const cmdline::parser &parser) {
  using tile_t = tile::BitTile;
  using tile_ex = CacheTile<tile_t>;
  using tiles_t = std::vector<tile_ex>;
  using board_t = board::BitBoard;
  using board_ex = util::EBoard<board_t>;
  using boards_t = std::vector<board_ex>;

  const unsigned long BEAM_WIDTH = parser.get<int>("beam_width");
  const int N = home.tiles.size();

  orliv::ZobristHash<std::uint64_t, 32, 32, 2> zobrist;
  Evaluater evaluater(home);

  board_ex initial(board_t(home.board));
  tiles_t tiles;
  int id = 0;
  for (const auto &tile : home.tiles) {
    tiles.emplace_back(tile_t(tile));
    tiles.back().fill(id++);
  }
  boards_t beam;
  beam.emplace_back(initial);
  beam.reserve(BEAM_WIDTH);
  boards_t nxt;
  nxt.reserve(550000);

  std::unordered_set<std::uint64_t> vis;
  vis.reserve(550000);

  auto best = initial;
  auto evalf = [&evaluater](board_ex &b) { b.eval = evaluater.eval(b); };
  while (beam.size()) {
    vis.clear();
    nxt.clear();
    benchmark("next beam generate") {
      for (const auto &b : beam) {
#ifdef BEAM_BENCH
        benchmark("beam") {
#endif
          for (int i = 0; i < N; i++) {
            if (b.isUsed(i)) continue;

            int tc = 0;
            for (const auto &tile : tiles[i].iter()) {
              for (int y = -7; y < 32; y++) {
                for (int x = -7; x < 32; x++) {
                  if (!b.canPut(tile, x, y)) continue;
                  if (!b.canPutStrong(tile, x, y)) continue;
                  auto nb = b;
                  nb.put(tile, x, y);
                  nb.useTile(i);
                  if (vis.count(nb.hashv)) continue;
                  vis.insert(nb.hashv);
                  nxt.emplace_back(std::move(nb));
                }
              }
              tc++;
              if (tile.zk == 1u) break;
              if (tile.zk == 2u && tc == 2) break;
              if (tile.zk == 3u && tc == 4) break;
            }
          }
#ifdef BEAM_BENCH
        }
#endif
      }
    }
    std::cerr << "nxt size:" << nxt.size() << std::endl;
    benchmark("min el") {
      auto bbest = std::min_element(nxt.begin(), nxt.end(),
                                    [](const board_ex &a, const board_ex &b) {
        return a.blanks() < b.blanks();
      });
      if (bbest != nxt.end() && bbest->blanks() < best.blanks()) best = *bbest;
    }

    if (nxt.size() > BEAM_WIDTH) {
      benchmark("eval") { std::for_each(nxt.begin(), nxt.end(), evalf); }
      std::partial_sort(nxt.begin(), nxt.begin() + BEAM_WIDTH, nxt.end());
      nxt.erase(nxt.begin() + BEAM_WIDTH, nxt.end());
    }
    beam = nxt;
    util::dumpBoard(best);
  }
  util::dumpBoard(best, true);
  return Answer();
}

cmdline::parser Beam::getParser() {
  cmdline::parser parser;
  parser.add<int>("beam_width", 'b', "beam width", false, 5);
  parser.add("verbose", 0, "show debug print");
  return parser;
}
}
}
