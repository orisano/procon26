#pragma once
#include "../Answer.hpp"
#include "../Home.hpp"
#include "../external/cmdline/cmdline.h"

namespace procon26 {
namespace takumi {

struct Jin {
  static cmdline::parser getParser();
  Answer solve(const Home &home, const int millisec,
               const cmdline::parser &parser);
};
}
}
