#include <fstream>
#include <cstdint>
#include "ZobristHash.hpp"

int main()
{
  std::ofstream ofs("HashValue.dat");
  orliv::ZobristHash<std::uint64_t, 32, 32, 2> zh;

  zh.output(ofs);

  return 0;
}
