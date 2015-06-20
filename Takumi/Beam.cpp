#include "Beam.hpp"
#include <algorithm>
#include <cstdio>
#include <chrono>
#include <set>
#include "../Tile.hpp"
#include "../CacheTile.hpp"
#include "ZobristHash.hpp"
#include "../../../orliv/benchmark.hpp"

#define BEAM_BENCH

namespace {
const int dx[] = {0, 0, 1, -1};
const int dy[] = {1, -1, 0, 0};
const int BEAM_WIDTH = 20;
const int ONE_STEP = 200;
const int SIZE = 32;
const int tri[] = {1, 3, 9, 27, 81};

int evalBoard(const procon26::Board &board) {
    int maxi = 0;
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            int c = board.data[y][x];
            if (maxi < c) maxi = c;
        }
    }

    int puttable_count = 0;
    int vis[SIZE][SIZE] = {};
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (board.data[y][x] < 1) continue;
            for (int d = 0; d < 4; d++) {
                int nx = x + dx[d], ny = y + dy[d];
                if (!board.inBounds(nx, ny)) continue;
                if (board.data[ny][nx] != 0) continue;
                puttable_count += !vis[ny][nx]++;
            }
        }
    }
    int density = 0;
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            density += tri[vis[y][x]];
        }
    }
    return board.zk + density + maxi * 5;
}

void dumpBoard(const procon26::Board &board) {
    puts("----------------");
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            printf("%4d", board.data[i][j]);
        }
        puts("");
    }
    printf("blanks: %d\n", board.zk);
    puts("----------------");
}
}

struct EBoard : public procon26::Board {
    typedef procon26::Board Derived;

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

Answer Beam::solve(const Home &home, int millisec) {
    Board initial = home.board;
    std::vector<CacheTile<Tile>> tiles;
    int id = 2;
    for (auto &tile : home.tiles) tiles.emplace_back(tile), tiles.back().fill(id++);

    std::vector<EBoard> beam;
    beam.emplace_back(initial);
    /*
     * あとで重複削除入れる
     * boardの状態と使ったピースのidでzobristhashで殴る
     */
    orliv::ZobristHash<std::uint64_t, 32, 32, 2> zb;

    const auto TILE_SIZE = tiles.size();
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<EBoard> nxt;
    nxt.reserve(700000);
    std::set<std::uint64_t> vis;
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
                        auto tile = tiles[i];
                        for (int y = -7; y < 31; y++) {
                            for (int x = -7; x < 31; x++) {
                                for (int r = 0; r < 4; r++, tile.rotate()) {
                                    if (!b.puttable(tile.value(), x, y)) continue;
                                    auto nb = b;
                                    nb.put(tile.value(), x, y);
                                    nb.useTile(i);
                                    auto hashv = zb.hash(nb);
                                    if (vis.count(hashv)) continue;
                                    vis.insert(hashv);
                                    nxt.push_back(nb);
                                }
                                tile.reverse();
                                for (int r = 0; r < 4; r++, tile.rotate()) {
                                    if (!b.puttable(tile.value(), x, y)) continue;
                                    auto nb = b;
                                    nb.put(tile.value(), x, y);
                                    nb.useTile(i);
                                    auto hashv = zb.hash(nb);
                                    if (vis.count(hashv)) continue;
                                    vis.insert(hashv);
                                    nxt.push_back(nb);
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
            benchmark("sort") {
                std::partial_sort(begin(nxt), begin(nxt) + BEAM_WIDTH, end(nxt), [](const EBoard &a, const EBoard &b) {
                    return a.eval < b.eval;
                });
            }
            benchmark("erase") {
                nxt.erase(begin(nxt) + BEAM_WIDTH, end(nxt));
            }
        }
        else {
            auto bbest = std::min_element(begin(nxt), end(nxt),
                                          [](const EBoard &a, const EBoard &b) { return a.blanks() < b.blanks(); });
            if (bbest != std::end(nxt) && bbest->blanks() < best.blanks()) best = *bbest;
        }
        benchmark("swap") {
            std::swap(beam, nxt);
        }
    }
    dumpBoard(best);
    return Answer();
}

}
}
