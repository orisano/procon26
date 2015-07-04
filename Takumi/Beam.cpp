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
const int dx[] = {0, 0, 1, -1, 1, 1, -1, -1};
const int dy[] = {1, -1, 0, 0, 1, -1, 1, -1};
const int ONE_STEP = 200;
const int SIZE = 32;
const int MAX_DIV = 4;
// const int tri[] = {0, 1, 4, 10, 12};
const int pn[] = {0, 1, 3, 7, 10, 14, 18, 22, 26};

using procon26::board::Board;

int evalBoard(const Board &board) {
    int maxi = 0;
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            int c = board.data[y][x];
            if (maxi < c) maxi = c;
        }
    }

    int density = 0;
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (board.data[y][x] != 0) continue;
            int cnt = 0;
            for (int d = 0; d < 8; d++) {
                int nx = x + dx[d], ny = y + dy[d];
                if (!board.inBounds(nx, ny)){
                    cnt++;
                }
                else if (board.data[ny][nx] != 0) {
                    cnt++;
                }
            }
            density += pn[cnt];

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

bool calcCenterOfGravity(const Board& board, int div, int& gx, int& gy){

    int min_x = Board::SIZE + 1, min_y = Board::SIZE + 1;
    int max_x = -1, max_y = -1;
    int sum_x = 0, sum_y = 0;
    int filled = 0;

    for (int y = 0; y < Board::SIZE; y++){
        for (int x = 0; x < Board::SIZE; x++){
            auto c = board.at(x, y);
            if (c == 1) continue;
            min_x = std::min(min_x, x);
            max_x = std::max(max_x, x);
            min_y = std::min(min_y, y);
            max_y = std::max(max_y, y);
            if (c == 0) continue;
            filled++;
            sum_x += x;
            sum_y += y;
        }
    }
    if (!filled) {
        gx = 0;
        gy = 0;
        return false;
    }

    sum_x /= filled;
    sum_y /= filled;

    const int rx = max_x - min_x;
    const int ry = max_y - min_y;
    const int tx = rx / div + 1;
    const int ty = ry / div + 1;

    gx = (sum_x - min_x) / tx;
    gy = (sum_y - min_y) / ty;

    return true;
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
    const int DIV = parser.get<int>("divide");
    const bool VERBOSE = parser.exist("verbose");

    Board initial = home.board;
    std::vector<CacheTile<Tile>> tiles;
    int id = 2;
    for (auto &tile : home.tiles) tiles.emplace_back(tile), tiles.back().fill(id++);

    std::vector<EBoard> beam[MAX_DIV][MAX_DIV];
    beam[0][0].emplace_back(initial);
    orliv::ZobristHash<std::uint64_t, 32, 32, 2> zb;

    const auto TILE_SIZE = tiles.size();
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<EBoard> nxt[MAX_DIV][MAX_DIV];
    for (int i = 0; i < DIV; i++) for (int j = 0; j < DIV; j++) nxt[i][j].reserve(100000);
    std::unordered_set<std::uint64_t> vis;
    vis.reserve(700000);
    auto best = initial;

    while (1) {
        for (int yi = 0; yi < DIV; yi++){
            for (int xi = 0; xi < DIV; xi++){
                //dumpBoard(beam[0]);
                //dumpBoard(best);
                vis.clear();
                benchmark("next beam generate") {
                    for (const auto &b : beam[yi][xi]) {
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
#if 1
                                            auto hashv = zb.hash(nb);
                                            if (vis.count(hashv)) continue;
                                            vis.insert(hashv);
#endif
                                            int gx, gy;
                                            calcCenterOfGravity(nb, DIV, gx, gy);
                                            nxt[gy][gx].emplace_back(nb);
                                        }
                                        tile.reverse();
                                        for (int r = 0; r < 4; r++, tile.rotate()) {
                                            if (!b.canPut(tile.value(), x, y)) continue;
                                            EBoard nb = b;
                                            nb.put(tile.value(), x, y);
                                            nb.useTile(i);
#if 1
                                            auto hashv = zb.hash(nb);
                                            if (vis.count(hashv)) continue;
                                            vis.insert(hashv);
#endif
                                            int gx, gy;
                                            calcCenterOfGravity(nb, DIV, gx, gy);
                                            nxt[gy][gx].emplace_back(nb);
                                        }
                                    }
                                }
                            }
#ifdef BEAM_BENCH
                        }
#endif
                    }
                }
            }
        }
        for (int yi = 0; yi < DIV; yi++) {
            for (int xi = 0; xi < DIV; xi++) {
                if (nxt[yi][xi].size() > BEAM_WIDTH) {
                    benchmark("eval") {
                        for (auto &b : nxt[yi][xi]) {
                            b.eval = evalBoard(b);
                            if (b.blanks() < best.blanks()) best = b;
                        }
                    }
                    std::partial_sort(begin(nxt[yi][xi]), begin(nxt[yi][xi]) + BEAM_WIDTH, end(nxt[yi][xi]),
                                      [](const EBoard &a, const EBoard &b) {
                                          return a.eval < b.eval;
                                      });

                    nxt[yi][xi].erase(begin(nxt[yi][xi]) + BEAM_WIDTH, end(nxt[yi][xi]));
                }
                else {
                    auto bbest = std::min_element(begin(nxt[yi][xi]), end(nxt[yi][xi]),
                                                  [](const EBoard &a, const EBoard &b) {
                                                      return a.blanks() < b.blanks();
                                                  });
                    if (bbest != std::end(nxt[yi][xi]) && bbest->blanks() < best.blanks()) best = *bbest;
                }
            }
        }

        std::swap(beam, nxt);
        size_t ss = 0;
        for (int i = 0; i < DIV; i++){
            for (int j = 0; j < DIV; j++){
                ss += beam[i][j].size();
                printf("%lu ", beam[i][j].size());
                nxt[i][j].clear();
            }
            puts("");
        }
        dumpBoard(best);
        if (!ss) break;
    }
    dumpBoard(best, true);
    return Answer();
}

cmdline::parser Beam::getParser() {
    cmdline::parser parser;
    parser.add<int>("beam_width", 'b', "beam width", false, 5);
    parser.add<int>("divide", 'd', "divide", false, 1, cmdline::range(1, MAX_DIV));
    parser.add("verbose", 0, "show debug print");
    return parser;
}
}
}
