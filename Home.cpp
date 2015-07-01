#include "Home.hpp"
#include "Action.hpp"
#include <fstream>
#include <string>
#include <cassert>

namespace procon26 {

void Home::load(const std::string &filepath) {
    std::ifstream ifs(filepath);
    assert(!ifs.fail() && "quest file not found");

    std::vector<std::string> lines;
    while (!ifs.eof()) {
        std::string line;
        ifs >> line;
        lines.emplace_back(line);
    }
    auto iter = lines.begin();
    iter = board.load(iter);
    const auto N = atoi((*iter).c_str());
    ++iter;
    tiles.clear();
    for (auto i = 0; i < N; ++i) {
        tiles.emplace_back();
        iter = tiles.back().load(iter);
    }
}

tile::Tile Home::toTile(Action action) const {
    auto tile = tiles[action.id];
    if (action.inverse) tile.reverse();
    for (auto i = 0; i < action.rotate; ++i) tile.rotate();
    tile.fill(action.id + 2);
    return tile;
}

}