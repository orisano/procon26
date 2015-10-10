#include "BitBeam.hpp"
#include "Util.hpp"
#include <algorithm>
#include <cstdio>
#include <chrono>
#include <set>
#include <unordered_set>
#include "../BitBoard.hpp"
#include "../BitTile.hpp"
#include "../CacheTile.hpp"
#include "../external/orliv/benchmark.hpp"
#include "../external/orliv/uf.hpp"
#include "../external/orliv/bool_array.hpp"
#include "../external/cmdline/cmdline.h"
#include <functional>

namespace {
const std::array<int, 24> dx = {0, 0, 1,  -1, 1, 1,  -1, -1, 2, -2, 0, 0,
                                2, 2, -2, -2, 1, -1, 1,  -1, 3, -3, 0, 0};
const std::array<int, 24> dy = {1, -1, 0, 0,  1, -1, -1, 1,  0, 0, 2, -2,
                                1, -1, 1, -1, 2, 2,  -2, -2, 0, 0, 3, -3};
const int SIZE = 32;
// const int pn[] = {0, 1, 3, 7, 13};
const int pn[] = {0, 1, 3, 7, 20};
// const int pn[] = {0, 1, 1, 1, 1};
// const int pn[] = {0, 1, 5, 20, 20};
// const int pn[] = {0, 1, 3, 7, 12};
// const int pn[] = {-1, -1, 3, 9, 27};

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
  int maxi = 0;
  int mini = 10000;
  for (int y = 0; y < SIZE; y++) {
    for (int x = 0; x < SIZE; x++) {
      if (board.at(x, y) == 0) {
        auto p = uf.root(x, y);
        if (!ba.get(p)) {
          int k = uf.size(x, y);
          res++;
          ba.set(p);
          mini = std::min(mini, k);
          maxi = std::max(maxi, k);
        }
      }
    }
  }
  return res;
}

inline int evalBoard(const BitBoard &board) {
#if 0
  static orliv::BoolArray<2048> ba;
  ba.clear();
  int cnt = 0;
  for (int y = 0; y < SIZE; y++) {
    for (int x = 0; x < SIZE; x++) {
      if (board.at(x, y) < 2) continue;
      for (int d = 0; d < dx.size(); d++) {
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
#else
#if 0 
  static orliv::BoolArray<2048> vis;
  vis.clear();
  std::function<int(int, int, int)> dfs = [&](int x, int y, int depth) {
    if (depth == 0) return 0;
    if (!board.inBounds(x, y)) return 0;
    if (vis.get(y * SIZE + x)) return 0;
    if (board.at(x, y) != 0) return 0;
    vis.set(y * SIZE + x);
    int res = 1;
    for (int d = 0; d < 4; d++) {
      const int nx = x + dx[d], ny = y + dy[d];
      res += dfs(nx, ny, depth - 1);
    }
    return res;
  };
  int cnt = 0;
  for (int y = 0; y < SIZE; y++) {
    for (int x = 0; x < SIZE; x++) {
      if (board.at(x, y) < 2) continue;
      for (int d = 0; d < 4; d++) {
        const int nx = x + dx[d], ny = y + dy[d];
        cnt += dfs(nx, ny, 5);
      }
    }
  }
  return -cnt + board.blanks();
#else
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
  // area = getArea(board);
  return board.blanks() + density + 50 * area;
#endif
#endif
}
}

namespace procon26 {
namespace takumi {

Answer BitBeam::solve(const Home &home, int millisec, cmdline::parser &parser) {
  using tile_t = tile::BitTile;
  using tile_ex = CacheTile<tile_t>;
  using tiles_t = std::vector<tile_ex>;
  using board_t = board::BitBoard;
  using board_ex = util::EBoard<board_t>;
  using boards_t = std::vector<board_ex>;

  const std::size_t BEAM_WIDTH = parser.get<int>("beam_width");
  const std::size_t N = home.tiles.size();

  const auto evalf = [](board_ex &b) {
    if (b.eval == -1) b.eval = evalBoard(b);
  };
  const auto comp_blank = [](
      const board_ex &a, const board_ex &b) { return a.blanks() < b.blanks(); };

  board_ex initial(board_t(home.board));
  tiles_t tiles;

  int id = 0;
  for (auto &tile : home.tiles) {
    tiles.emplace_back(tile_t(tile));
    tiles.back().fill(id++);
  }
  boards_t beam, nxt;

  evalf(initial);
  beam.emplace_back(initial);
  nxt.reserve(550000);

  std::unordered_set<std::uint64_t> vis;
  vis.reserve(550000);

  auto best = initial;

  std::size_t tile_id = 0;
  while ((tile_id < N && beam.size())) {
    std::fprintf(stderr, "tile_id: %lu / %lu\n", tile_id, tiles.size());

    nxt.clear();
    vis.clear();
    util::dumpBoard(best);
    benchmark("next beam generate") {
      for (const auto &b : beam) {
        nxt.emplace_back(b);

        for (const auto &tile : tiles[tile_id].iter()) {
          for (int y = -7; y < 32; y++) {
            for (int x = -7; x < 32; x++) {
              if (!b.canPut(tile, x, y)) continue;
              auto nb = b;
              nb.put(tile, x, y);
              nb.useTile(tile_id);
              if (vis.count(nb.hashv)) continue;
              vis.insert(nb.hashv);
              int e = b.eval;
              for (int ty = 0; ty < 8; ty++) {
                for (int tx = 0; tx < 8; tx++) {
                  if (!tile.at(tx, ty)) continue;
                  const int cx = tx + x, cy = ty + y;
                  int cnt = 0;
                  for (int d = 0; d < 4; d++) {
                    const int nx = cx + dx[d], ny = cy + dy[d];
                    if (!b.inBounds(nx, ny) || b.at(nx, ny) != 0) {
                      cnt++;
                    }

                    int ccnt = 0;
                    int ncnt = 0;
                    for (int dd = 0; dd < 4; dd++) {
                      const int nnx = nx + dx[dd], nny = ny + dy[dd];
                      if (!b.inBounds(nnx, nny) || b.at(nnx, nny) != 0) {
                        ccnt++;
                      }
                      if (!nb.inBounds(nnx, nny) || nb.at(nnx, nny) != 0) {
                        ncnt++;
                      }
                    }
                    ccnt *= !(!b.inBounds(nx, ny) || b.at(nx, ny) != 0);
                    ncnt *= !(!nb.inBounds(nx, ny) || nb.at(nx, ny) != 0);
                    e -= pn[ccnt];
                    e += pn[ncnt];
                  }
                  e -= pn[cnt];
                }
              }
              e -= tile.zk;
              nb.eval = e;
              nxt.emplace_back(std::move(nb));
            }
          }
        }
      }
    }
    auto bbest = std::min_element(nxt.begin(), nxt.end(), comp_blank);
    if (bbest != nxt.end() && bbest->blanks() < best.blanks()) {
      best = *bbest;
    }
    std::fprintf(stderr, "nxt size: %lu\n", nxt.size());
    if (nxt.size() > BEAM_WIDTH) {
      /*
      benchmark("eval") { std::for_each(nxt.begin(), nxt.end(), evalf); }
      */
      benchmark("sort") {
        std::partial_sort(nxt.begin(), nxt.begin() + BEAM_WIDTH, nxt.end());
      }
      nxt.erase(nxt.begin() + BEAM_WIDTH, nxt.end());
    }
    beam = nxt;
    tile_id++;
  }
  util::dumpBoard(best, true);
  return Answer();
}

cmdline::parser BitBeam::getParser() {
  cmdline::parser parser;
  parser.add<int>("beam_width", 'b', "beam width", false, 1000);

  return parser;
}
}
}
