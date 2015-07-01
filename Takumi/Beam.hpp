#pragma once
#include "Takumi.hpp"
#include "../Answer.hpp"
#include "../Home.hpp"
#include "../external/cmdline/cmdline.h"

namespace procon26 {
namespace takumi {

struct Beam {
    static cmdline::parser getParser();
    Answer solve(const Home &home, const int millisec, const cmdline::parser& parser);
};

}
}
