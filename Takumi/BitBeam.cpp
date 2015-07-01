#include "BitBeam.hpp"
#include <algorithm>
#include <cstdio>
#include <chrono>
#include <set>
#include "../BitBoard.hpp"
#include "../BitTile.hpp"
#include "../CacheTile.hpp"
#include "ZobristHash.hpp"
#include "../../../orliv/benchmark.hpp"

// #define BEAM_BENCH

namespace {
const int dx[] = {0, 0, 1, -1};
const int dy[] = {1, -1, 0, 0};
const int BEAM_WIDTH = 2000;
const int ONE_STEP = 200;
const int SIZE = 32;
const int tri[] = {-1, -1, 3, 9, 27};

using procon26::board::BitBoard;

int dfs(int data[SIZE][SIZE], int x, int y)
{
    if (0 > x || x >= SIZE || 0 > y || y >= SIZE) return 0;
    if (data[y][x] == 1) return 0;
    data[y][x] = 1;
    int sum = 1;
    for (int d = 0; d < 4; d++){
        sum += dfs(data, x + dx[d], y + dy[d]);
    }
    return sum;
}


int evalBoard(const BitBoard &board) {
    int vis[SIZE][SIZE] = {};
    int data[SIZE][SIZE];
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            data[y][x] = 0;
            if (!board.at(x, y)) continue;
            data[y][x] = 1;
            for (int d = 0; d < 4; d++) {
                int nx = x + dx[d], ny = y + dy[d];
                if (!board.inBounds(nx, ny)) continue;
                if (board.at(nx, ny) || (board.initial[ny] >> nx & 1)) continue;
                vis[ny][nx]++;
            }
        }
    }

    std::vector<int> vs;
    double avg = 0;
    for (int y = 0; y < SIZE; y++){
        for (int x = 0; x < SIZE; x++){
            if (data[y][x]) continue;
            auto rv = dfs(data, x, y);
            avg += rv;
            vs.push_back(rv);
        }
    }
    const int N = vs.size();
    const int MIN_V = *std::min_element(vs.begin(), vs.end());
    const int MAX_V = *std::max_element(vs.begin(), vs.end());
    /*
    avg /= N;
    double variance = 0;
    for (const int v : vs){
        variance += (v - avg);
    }
    variance /= N - 1;
    */
    int density = 0;
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            density += tri[vis[y][x]];
        }
    }
    return (board.blanks() + density) + 32 * N + MAX_V / MIN_V;
}

void dumpBoard(const BitBoard &board) {
    std::puts("----------------");
    for (int i = 0; i < 32; i++) {
        auto init = board.initial[i];
        auto curr = board.data[i];
        for (int j = 0; j < 32; j++) {
            std::printf("%4d", ((init >> j) & 1) * 2 | ((curr >> j) & 1));
        }
        std::puts("");
    }
    std::printf("blanks: %d\n", board.blanks());
    std::puts("----------------");
}
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


namespace procon26 {
namespace takumi {

Answer BitBeam::solve(const Home &home, int millisec) {
    using tile::BitTile;
    BitBoard initial(home.board);
    std::vector<CacheTile<BitTile> > tiles;
    int id = 2;
    for (auto &tile : home.tiles) tiles.emplace_back(BitTile(tile)), tiles.back().fill(id++);

    std::vector<EBoard> beam;
    beam.emplace_back(initial);
    /*
     * あとで重複削除入れる
     * boardの状態と使ったピースのidでzobristhashで殴る
     */
    orliv::ZobristHash<std::uint64_t, 32, 32, 2> zb;

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<EBoard> nxt;
    nxt.reserve(700000);
    std::set<std::uint64_t> vis;
    auto best = initial;
    int tile_id = 0;
    while ((tile_id < tiles.size() && beam.size())){ // std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() + ONE_STEP < millisec) {
        printf("tile_id: %d / %lu\n", tile_id, tiles.size());
        dumpBoard(beam[0]);
        //dumpBoard(best);
        nxt.clear();
        vis.clear();
        int beam_count = 0;
        benchmark("next beam generate") {
            for (const auto &b : beam) {
                if (beam_count++ < (BEAM_WIDTH / 10)) nxt.push_back(b);
#ifdef BEAM_BENCH
                benchmark("beam") {
#endif
                    auto tile = tiles[tile_id];
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
                                nxt.push_back(nb);
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
                                nxt.push_back(nb);
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
            nxt.erase(begin(nxt) + BEAM_WIDTH, end(nxt));
        }
        else {
            auto bbest = std::min_element(begin(nxt), end(nxt),
                                          [](const EBoard &a, const EBoard &b) { return a.zk < b.zk; });
            if (bbest != std::end(nxt) && bbest->blanks() < best.blanks()) best = *bbest;
        }
        std::swap(beam, nxt);
        tile_id++;
    }
    dumpBoard(best);
    return Answer();
}

}
}
