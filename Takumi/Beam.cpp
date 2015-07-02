#include "Beam.hpp"
#include <algorithm>
#include <cstdio>
#include <chrono>
#include <set>
#include <unordered_set>
#include "../Tile.hpp"
#include "../CacheTile.hpp"
#include "ZobristHash.hpp"
#include "../../../orliv/benchmark.hpp"
#include "../external/cmdline/cmdline.h"

#define BEAM_BENCH

namespace {
const int dx[] = {0, 0, 1, -1};
const int dy[] = {1, -1, 0, 0};
const int ONE_STEP = 200;
const int SIZE = 32;
const int tri[] = {0, 1, 4, 10, 12};

using procon26::board::Board;

int evalBoard(const Board &board) {
    int maxi = 0;
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            int c = board.data[y][x];
            if (maxi < c) maxi = c;
        }
    }

    int vis[SIZE][SIZE] = {};
    int mini_neighbor = 0;
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (board.data[y][x] < 1) continue;
            for (int d = 0; d < 4; d++) {
                int nx = x + dx[d], ny = y + dy[d];
                if (!board.inBounds(nx, ny)) continue;
                if (board.data[ny][nx] != 0) continue;
                vis[ny][nx]++;
                // mini_neighbor += board.data[y][x];
            }
        }
    }
    int density = 0;
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            density += tri[vis[y][x]];
        }
    }
    return board.blanks() + density + maxi;
}

int getColor(int c){
    if (c == 0) return 47;
    if (c == 1) return 40;
    return (c - 2) % 6 + 41;
}

void dumpBoard(const Board &board, bool number=false) {
    puts("----------------");
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            if (number) printf("%4d", board.data[i][j]);
            else printf("\x1b[%dm  \x1b[49m", getColor(board.data[i][j]));
        }
        puts("");
    }
    printf("blanks: %d\n", board.blanks());
    puts("----------------");
}
}

struct EBoard : public Board {
    using Derived = Board;

    EBoard() : Derived(), eval(0) {
        used_[0] = used_[1] = used_[2] = used_[3] = 0;
    }

    EBoard(Derived b) : Derived(b), eval(0) { }

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


namespace procon26 {
namespace takumi {

Answer Beam::solve(const Home &home, const int millisec, const cmdline::parser& parser) {
    using tile::Tile;

    const int BEAM_WIDTH = parser.get<int>("beam_width");
    const bool VERBOSE = parser.exist("verbose");

    Board initial = home.board;
    std::vector<CacheTile<Tile>> tiles;
    int id = 2;
    for (auto &tile : home.tiles) tiles.emplace_back(tile), tiles.back().fill(id++);

    std::vector<EBoard> beam;
    beam.emplace_back(initial);
    orliv::ZobristHash<std::uint64_t, 32, 32, 2> zb;

    const auto TILE_SIZE = tiles.size();
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<EBoard> nxt;
    nxt.reserve(700000);
    std::unordered_set<std::uint64_t> vis;
    vis.reserve(700000);
    auto best = initial;
    while (beam.size() || std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start).count() + ONE_STEP < millisec) {
        dumpBoard(beam[0]);
        dumpBoard(best);
        nxt.clear();
        vis.clear();
        benchmark("next beam generate") {
            for (const auto &b : beam) {
#ifdef BEAM_BENCH
                benchmark("beam") {
#endif
                    for (int i = 0; i < TILE_SIZE; i++) {
                        if (b.isUsed(i)) continue;
                        auto& tile = tiles[i];
                        for (int y = -7; y < 32; y++) {
                            for (int x = -7; x < 32; x++) {
                                for (int r = 0; r < 4; r++, tile.rotate()) {
                                    if (!b.canPut(tile.value(), x, y)) continue;
                                    EBoard nb = b;
                                    nb.put(tile.value(), x, y);
                                    nb.useTile(i);
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
                                    nb.useTile(i);
                                    auto hashv = zb.hash(nb);
                                    if (vis.count(hashv)) continue;
                                    vis.insert(hashv);
                                    nxt.emplace_back(nb);
                                }
                            }
                        }
                    }
#ifdef BEAM_BENCH
                }
#endif
            }
        }
        printf("nxt size: %lu\n", nxt.size());
        if (nxt.size() > BEAM_WIDTH) {
            benchmark("eval") {
                for (auto &b : nxt) {
                    b.eval = evalBoard(b);
                    if (b.blanks() < best.blanks()) best = b;
                }
            }
            std::partial_sort(begin(nxt), begin(nxt) + BEAM_WIDTH, end(nxt), [](const EBoard &a, const EBoard &b) {
                return a.eval < b.eval;
            });

            nxt.erase(begin(nxt) + BEAM_WIDTH, end(nxt));
        }
        else {
            auto bbest = std::min_element(begin(nxt), end(nxt),
                                          [](const EBoard &a, const EBoard &b) { return a.blanks() < b.blanks(); });
            if (bbest != std::end(nxt) && bbest->blanks() < best.blanks()) best = *bbest;
        }
        std::swap(beam, nxt);
    }
    dumpBoard(best, true);
    return Answer();
}

cmdline::parser Beam::getParser() {
    cmdline::parser parser;
    parser.add<int>("beam_width", 'b', "beam width", false, 20);
    parser.add("verbose", 0, "show debug print");
    return parser;
}
}
}
