#ifndef INCLUDE_BOOL_ARRAY_HPP
#define INCLUDE_BOOL_ARRAY_HPP

#include <cstring>
#include <climits>
#include <cassert>

namespace orliv {
template<unsigned int CAP>
struct BoolArray {
  typedef int memory_t;
  memory_t K;
  memory_t data[CAP];
  BoolArray() { clear_deep(); }
  void clear_deep(){ std::memset(data, 0, sizeof(data)); K = 1; }
  void clear(){ assert(K < std::numeric_limits<memory_t>::max()); K++; }
  bool within_k(int x, int k){ return data[x] + k >= K; }
  bool get(int x){ return data[x] == K; }
  void set(int x){ data[x] = K; }
};
}

#endif
