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
const int pn[] = {0, 1, 3, 7, 12};

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

inline int evalBoard(const BitBoard &board) {
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
//            int cs = dfs(data, x, y);
//            if (1 <= cs && cs <= PENALTY_CLUSTER_SIZE) cluster++;
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

    EBoard() : Derived(), eval(-1) {
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

namespace procon26 {
namespace takumi {

Answer BitBeam::solve(const Home &home, int millisec, cmdline::parser& parser) {
    using tile::BitTile;

    const int BEAM_WIDTH = parser.get<int>("beam_width");

    BitBoard initial(home.board);
    std::vector <CacheTile<BitTile>> tiles;
    int id = 0;
    for (auto &tile : home.tiles) tiles.emplace_back(BitTile(tile)), tiles.back().fill(id++);

    std::vector <EBoard> beam;
    beam.emplace_back(initial);
    orliv::ZobristHash<std::uint64_t, 32, 32, 2> zb;

    // auto start = std::chrono::high_resolution_clock::now();
    std::vector <EBoard> nxt;
    nxt.reserve(1200000);
    std::unordered_set <std::uint64_t> vis;
    vis.reserve(1200000);

    auto best = initial;
    int tile_id = 0;
    while ((tile_id < (int)tiles.size() &&
            beam.size())) { // std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() + ONE_STEP < millisec) {
        printf("tile_id: %d / %lu\n", tile_id, tiles.size());
        //dumpBoard(beam[0]);
        dumpBoard(best);
        nxt.clear();
        vis.clear();
        benchmark("next beam generate") {
            for (const auto &b : beam) {
                nxt.emplace_back(b);
#ifdef BEAM_BENCH
                benchmark("beam") {
#endif
                auto& tile = tiles[tile_id];
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
                            nxt.emplace_back(nb);
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
                            nxt.emplace_back(nb);
                        }
                    }
                }
#ifdef BEAM_BENCH
                }
#endif
            }
        }
        printf("nxt size: %lu\n", nxt.size());
        if ((int)nxt.size() > BEAM_WIDTH) {
            benchmark("eval") {
                for (auto &b : nxt) {
                    b.eval = evalBoard(b);
                    if (b.blanks() < best.blanks()) best = b;
                }
            }
            benchmark("sort") {
                std::partial_sort(begin(nxt), begin(nxt) + BEAM_WIDTH, end(nxt), [](const EBoard &a, const EBoard &b) {
                    return a.eval < b.eval;
                });
            }
            nxt.erase(begin(nxt) + BEAM_WIDTH, end(nxt));
        }
        auto bbest = std::min_element(begin(nxt), end(nxt),
                                          [](const EBoard &a, const EBoard &b) { return a.blanks() < b.blanks(); });
        if (bbest != std::end(nxt) && bbest->blanks() < best.blanks()) best = *bbest;
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
