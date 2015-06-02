#pragma once
#include "Takumi.hpp"
#include "../Answer.hpp"
#include "../Home.hpp"

namespace procon26 {
namespace takumi {

struct Test : Takumi {
  Answer solve(const Home& home, const int millsec);
};

}
}
