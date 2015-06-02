#include "../Home.hpp"
#include <fstream>
#include <string>

namespace procon26 {

template<typename BoardType, typename TileType>
void HomeImpl<BoardType, TileType>::load(const std::string& filepath)
{
  std::ifstream ifs(filepath);
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

}