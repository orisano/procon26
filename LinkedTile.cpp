#include "LinkedTile.hpp"

using namespace std;

namespace procon26 {
namespace tile {

LinkedTile::LinkedTile(const Tile &tile) {
    cell_value = tile.cell_value;
    for (int y = 0; y < Tile::SIZE; y++){
        for (int x = 0; x < Tile::SIZE; x++){
            if (!tile.data[y][x]) continue;
            data.emplace_back(x, y);
        }
    }
}

void LinkedTile::reverse() {
    for (auto& point : data) point.x = SIZE - point.x - 1;
}

void LinkedTile::rotate() {
    for (auto& point : data) point = Point(point.y, SIZE - point.x - 1);
}

LinkedTile::size_type LinkedTile::blanks() const {
    return static_cast<size_type>(SIZE * SIZE - data.size());
}

LinkedTile::cell_type LinkedTile::at(int x, int y) const {
    auto iter = find(begin(data), end(data), Point(x, y));
    return iter != end(data) ? cell_value : static_cast<cell_type>(0);
}

}
}