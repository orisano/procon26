#include <cassert>

bool naive(int x, int y) { return 0 <= x && x < 32 && 0 <= y && y < 32; }
bool inbounds(int x, int y) { return !(~31 & (x | y)); }

int main() {
  for (int y = -100; y < 100; y++) {
    for (int x = -100; x < 100; x++) {
      assert(naive(x, y) == inbounds(x, y));
    }
  }

  return 0;
}
