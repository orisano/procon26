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
#include "../../../orliv/benchmark.hpp"
#include "../external/cmdline/cmdline.h"

// #define BEAM_BENCH

namespace {
const int dx[] = {0, 0, 1, -1};
const int dy[] = {1, -1, 0, 0};
const int SIZE = 32;
const int pn[] = {0, 1, 3, 7, 7};

using procon26::board::BitBoard;

inline int dfs(int data[SIZE][SIZE], int x, int y) {
    if (0 > x || x >= SIZE || 0 > y || y >= SIZE) return 0;
    if (data[y][x] == 1) return 0;
    data[y][x] = 1;
    int sum = 1;
    for (int d = 0; d < 4; d++) {
        sum += dfs(data, x + dx[d], y + dy[d]);
    }
    return sum;
}

const int PENALTY_CLUSTER_SIZE = 5;

int evalBoard(const BitBoard &board) {
    int data[SIZE][SIZE] = {};
    for (int i = 0; i < SIZE; i++){
      for (int j = 0; j < SIZE; j++){
        data[i][j] = board.at(j, i) != 0;
      }
    }
    int density = 0;
    int cluster = 0;
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (board.at(x, y)) continue;
            int cs = dfs(data, x, y);
            if (1 <= cs && cs <= PENALTY_CLUSTER_SIZE) cluster++;
            int cnt = 0;
            for (int d = 0; d < 4; d++) {
                int nx = x + dx[d], ny = y + dy[d];
                if (!board.inBounds(nx, ny)) {
                    cnt++;
                    continue;
                }
                if (board.at(nx, ny) == 0) continue;
                cnt++;
            }
            density += pn[cnt];
        }
    }

    return board.blanks() + density + 10 * cluster;
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
            if (number) printf("%4d", c);
            else printf("\x1b[%dm  \x1b[49m", getColor(c));
        }
        puts("");
    }
    printf("blanks: %d\n", board.blanks());
    puts("----------------");
}


struct EBoard : public BitBoard {
    using Derived = BitBoard;

    EBoard() : Derived(), eval(0) {
        used_[0] = used_[1] = used_[2] = used_[3] = 0;
    }

    EBoard(Derived b) : Derived(b), eval(0) {
        used_[0] = used_[1] = used_[2] = used_[3] = 0;
    }

    bool isUsed(int id) const {
        assert(0 <= id && id < 256);
        return ((used_[id >> 6] >> (id & 63)) & 1) != 0;
    }

    void useTile(int id) {
        assert(!isUsed(id));
        used_[id >> 6] |= 1ull << (id & 63);
    }

    int eval;
  private:
    std::uint64_t used_[4];
};
}

using namespace std;

namespace procon26 {
namespace takumi {

Answer BitBeam::solve(const Home &home, int millisec, cmdline::parser& parser) {
    using tile::BitTile;

    const int BEAM_WIDTH = parser.get<int>("beam_width");

    BitBoard initial(home.board);
    vector<CacheTile<BitTile>> tiles;
    int id = 0;
    for (auto &tile : home.tiles) tiles.emplace_back(BitTile(tile)), tiles.back().fill(id++);

    vector<EBoard> beam;
    beam.reserve(1200000);
    beam.emplace_back(initial);

    unordered_set<uint64_t> vis;
    vis.reserve(1200000);

    orliv::ZobristHash<uint64_t, 32, 32, 2> zb;

    auto best = initial;
    int tile_id = 0;
    while ((tile_id < (int)tiles.size() && beam.size())) {
        printf("tile_id: %d / %lu\n", tile_id, tiles.size());
        dumpBoard(best);
        vis.clear();

        const int beam_len = beam.size();

        benchmark("next beam generate") {
            for (int bi = 0; bi < beam_len; bi++) {
                const auto &b = beam[bi];
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
                            auto hashv = zb.hash(nb);
                            if (vis.count(hashv)) continue;
                            vis.insert(hashv);
                            beam.emplace_back(nb);
                        }
                        tile.reverse();
                        for (int r = 0; r < 4; r++, tile.rotate()) {
                            if (!b.canPut(tile.value(), x, y)) continue;
                            EBoard nb = b;
                            nb.put(tile.value(), x, y);
                            nb.useTile(tile_id);
                            auto hashv = zb.hash(nb);
                            if (vis.count(hashv)) continue;
                            vis.insert(hashv);
                            beam.emplace_back(nb);
                        }
                    }
                }
#ifdef BEAM_BENCH
                }
#endif
            }
        }
        printf("nxt size: %lu\n", beam.size());
        if ((int) beam.size() > BEAM_WIDTH) {
            benchmark("eval") {
                for (int bi = beam_len; bi < (int) beam.size(); bi++) {
                    auto &b = beam[bi];
                    b.eval = evalBoard(b);
                    if (b.blanks() < best.blanks()) best = b;
                }
            }
            benchmark("sort") {
                partial_sort(begin(beam), begin(beam) + BEAM_WIDTH, end(beam),
                                  [](const EBoard &a, const EBoard &b) {
                                      return a.eval < b.eval;
                                  });
            }
            beam.erase(begin(beam) + BEAM_WIDTH, end(beam));
        }
        auto bbest = min_element(begin(beam), end(beam),
                                      [](const EBoard &a, const EBoard &b) { return a.blanks() < b.blanks(); });
        if (bbest != end(beam) && bbest->blanks() < best.blanks()) best = *bbest;
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
