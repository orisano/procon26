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
#include "../../../../orliv/benchmark.hpp"
#include "../external/cmdline/cmdline.h"

#define BEAM_BENCH

namespace {
const int dx[] = {0, 0, 1, -1, 1, 1, -1, -1};
const int dy[] = {1, -1, 0, 0, 1, -1, 1, -1};
const int ONE_STEP = 200;
const int SIZE = 32;
// const int tri[] = {0, 1, 4, 10, 12};
const int pn[] = {0, 1, 3, 7, 10, 14, 18, 22, 26};
// const int pn[] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

struct Evaluater {
  using Home = procon26::Home;

  Evaluater(const Home &home) : home(home) {}

  template <typename T>
  inline int eval(const T &board) const {
    int maxi = 0;
    for (int y = 0; y < SIZE; y++) {
      for (int x = 0; x < SIZE; x++) {
        int c = board.at(x, y);
        if (maxi < c) maxi = c;
      }
    }
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
          cnt += (board.at(nx, ny) != 0);
        }
        density += pn[cnt];
      }
    }

    return board.blanks() + density + maxi;
  }

  const Home home;
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

  const int BEAM_WIDTH = parser.get<int>("beam_width");
  const bool VERBOSE = parser.exist("verbose");
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
  boards_t beam, nxt;
  beam.emplace_back(initial);
  beam.reserve(BEAM_WIDTH);
  nxt.reserve(550000);

  std::unordered_set<std::uint64_t> vis;
  vis.reserve(550000);

  auto best = initial;

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
            for (int y = -7; y < 32; y++) {
              for (int x = -7; x < 32; x++) {
                for (const auto &tile : tiles[i].iter()) {
                  if (!b.canPut(tile, x, y)) continue;
                  if (!b.canPutStrong(tile, x, y)) continue;
                  auto nb = b;
                  nb.put(tile, x, y);
                  nb.useTile(i);
                  auto hashv = zobrist.hash(nb);
                  if (vis.count(hashv)) continue;
                  vis.insert(hashv);
                  nxt.emplace_back(std::move(nb));
                }
              }
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
      benchmark("eval") {
        for (auto &b : nxt) {
          b.eval = evaluater.eval(b);
        }
      }
      std::partial_sort(nxt.begin(), nxt.begin() + BEAM_WIDTH, nxt.end());
      nxt.erase(nxt.begin() + BEAM_WIDTH, nxt.end());
    }
    beam = nxt;
    util::dumpBoard(best, true);
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
