#include "Jin.hpp"
#include "Util.hpp"
#include "../BitBoard.hpp"
#include "../BitTile.hpp"
#include "../CacheTile.hpp"
#include "./ZobristHash.hpp"
#include "../../../../orliv/len_queue.hpp"
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>

namespace {

const int SIZE = 32;
const std::array<int, 24> dx = {0, 0, 1,  -1, 1, 1,  -1, -1, 2, -2, 0, 0,
                                2, 2, -2, -2, 1, -1, 1,  -1, 3, -3, 0, 0};
const std::array<int, 24> dy = {1, -1, 0, 0,  1, -1, -1, 1,  0, 0, 2, -2,
                                1, -1, 1, -1, 2, 2,  -2, -2, 0, 0, 3, -3};
const std::array<int, 5> pn = {0, 1, 3, 7, 20};

struct Evaluater {
  typedef procon26::Home Home;

  Evaluater(const Home& home) : home(home) {}

  template <typename T>
  int eval(const T& board) {
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
    return board.blanks() + density;
  }

  const Home home;
};
}

namespace procon26 {
namespace takumi {

Answer Jin::solve(const Home& home, const int millisec,
                  const cmdline::parser& parser) {
  using tile_t = tile::BitTile;
  using tile_ex = CacheTile<tile_t>;
  using board_t = board::BitBoard;
  using board_ex = util::EBoard<board_t>;

  std::cerr << "Solve start...\n";

  const int BEAM_WIDTH = parser.get<int>("beam_width");
  const int BEAM_TOP = parser.get<int>("beam_top");
  const int N = home.tiles.size();

  Evaluater evaluater(home);
  std::cerr << "Evaluater Initialized...\n";

  orliv::ZobristHash<std::uint64_t, 32, 32, 2> zb;
  std::cerr << "Hash Initialized...\n";

  int id = 0;
  std::vector<tile_ex> tiles;
  for (auto& tile : home.tiles) {
    tiles.emplace_back(tile_t(tile));
    tiles.back().fill(id++);
  }
  std::vector<int> tiles_zk(N + 1);
  for (int i = N - 1; i >= 0; i--) {
    tiles_zk[i] = tiles_zk[i + 1] + home.tiles[i].zk;
  }

  std::cerr << "Tile Initialized...\n";

  std::vector<orliv::LenQueue<board_ex>> pqs(N + 1, BEAM_WIDTH);
  std::cerr << "LenQueue's Initialized...\n";

  std::unordered_set<std::uint64_t> vis;
  vis.reserve(500000);

  board_t initb(home.board);
  board_ex bestb(initb);
  pqs[0].push(initb);
  while (1) {
    bool ok = false;
    std::cerr << "step...\n";

    vis.clear();
    for (int i = N - 1; i >= 0; i--) {
      if (pqs[i].empty()) continue;
      ok = true;
      for (int k = 0; k < BEAM_TOP; k++) {
        if (pqs[i].empty()) break;
        auto b = pqs[i].top();
        pqs[i].pop();
        for (int j = i; j < N; j++) {
          if (b.blanks() > tiles_zk[j]) continue;
          for (const auto& tile : tiles[j].iter()) {
            for (int y = -7; y < 32; y++) {
              for (int x = -7; x < 32; x++) {
                if (!b.canPut(tile, x, y)) continue;
                auto nb = b;
                nb.put(tile, x, y);
                nb.useTile(j);
                auto hashv = zb.hash(nb);
                if (vis.count(hashv)) continue;
                vis.insert(hashv);
                if (bestb.blanks() > nb.blanks()) {
                  bestb = nb;
                  util::dumpBoard(bestb, true);
                  util::dumpBoard(bestb);
                }
                nb.eval = evaluater.eval(nb);
                pqs[j + 1].push(nb);
              }
            }
          }
          // std::cerr << "processed " << j << "/" << N - 1 << "\n";
        }
      }
      while (!pqs[i].empty()) pqs[i].pop();
    }
    if (!ok) break;
  }
  util::dumpBoard(bestb, true);
  return Answer();
}

cmdline::parser Jin::getParser() {
  cmdline::parser parser;
  parser.add<int>("beam_width", 'b', "beam width", false, 100);
  parser.add<int>("beam_top", 0, "beam top", false, 5);
  return parser;
}
}
}
