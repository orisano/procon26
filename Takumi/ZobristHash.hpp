#ifndef INCLUDE_ZOBRIST_HASH_HPP
#define INCLUDE_ZOBRIST_HASH_HPP

#include <random>
#include <iostream>
#include <typeinfo>

namespace orliv {

template<typename T, int W, int H, int STATE>
struct ZobristHash {
  typedef T hash_t;
  ZobristHash(){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<T> dist;
    for (int h = 0; h < H; h++){
      for (int w = 0; w < W; w++){
        for (int s = 0; s < STATE; s++){
          rands[h][w][s] = dist(mt);
        }
      }
    }
  }
  template<typename TT>
  hash_t hash(const TT& data){
    hash_t r = 0;
    for (int y = 0; y < H; y++){
      for (int x = 0; x < W; x++){
        r ^= rands[y][x][data.at(x, y) > 0];
      }
    }
    return r;
  }
  template<typename OS>
  void output(OS& st){
    st << "#ifndef INCLUDE_HASH_VALUE\n";
    st << "#define INCLUDE_HASH_VALUE\n";
    st << "#include <cstdint>\n";
    st << "namespace orliv {\n";
    st << "using hash_type = std::uint64_t;\n";
    st << "const hash_type hash_v[" << H << "][" << W << "][" << STATE << "] = {\n";
    for (int h = 0; h < H; h++){
      st << "{";
      for (int w = 0; w < W; w++){
        st << "{";
        for (int s = 0; s < STATE; s++){
          st << rands[h][w][s] << "ULL,";
        }
        st << "},\n";
      }
      st << "},\n";
    }
    st << "};\n";
    st << "}\n";
    st << "#endif\n";
  }
  hash_t rands[H][W][STATE];
};
}

#endif
