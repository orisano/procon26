#pragma once

#include "../Home.hpp"
#include "../Answer.hpp"

namespace procon26 {
namespace takumi {

struct Takumi {
  virtual Answer solve(const Home& home, int millisec)=0;
};

}
}
