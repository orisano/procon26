#ifndef INCLUDE_TAKUMI_UTIL_HPP
#define INCLUDE_TAKUMI_UTIL_HPP

#include <cassert>
#include <cstdint>
#include <cstdio>

namespace procon26 {
namespace takumi {
namespace util {

template <typename T>
struct EBoard : public T {
  using Derived = T;

  EBoard() : Derived() {}

  EBoard(Derived b) : Derived(b) {}

  bool isUsed(int id) const {
    assert(0 <= id && id < 256);
    return ((used_[id >> 6] >> (id & 63)) & 1) != 0;
  }

  void useTile(int id) {
    eval = -1;
    assert(!isUsed(id));
    if (maxi < id) maxi = id;
    used_[id >> 6] |= 1ull << (id & 63);
  }

  bool operator<(const EBoard& rhs) const { return eval < rhs.eval; }
  bool operator>(const EBoard& rhs) const { return eval > rhs.eval; }
  bool operator==(const EBoard& rhs) const { return eval == rhs.eval; }

  int eval = -1;
  std::uint8_t maxi = 0;

 private:
  std::uint64_t used_[4] = {};
};

int getColor(int c) {
  if (c == 0) return 47;
  if (c == 1) return 40;
  return (c - 2) % 6 + 41;
}

template <typename T>
void dumpBoard(const T& board, bool number = false) {
  std::fprintf(stderr, "-----------------\n");
  for (int y = 0; y < T::SIZE; y++) {
    for (int x = 0; x < T::SIZE; x++) {
      auto c = board.at(x, y);
      if (number) {
        std::fprintf(stderr, "%4d", c);
      } else {
        std::fprintf(stderr, "\x1b[%dm  \x1b[49m", getColor(c));
      }
    }
    std::fprintf(stderr, "\n");
  }
  std::fprintf(stderr, "blanks: %d\n", board.blanks());
  std::fprintf(stderr, "-----------------\n");
}
}
}
}

#endif
