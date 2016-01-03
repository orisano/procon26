#pragma once
#include "Takumi.hpp"
#include "../Answer.hpp"
#include "../Home.hpp"
#include "../external/cmdline/cmdline.h"

namespace procon26 {
namespace takumi {

struct BitBeam {
    Answer solve(const Home &home, const int millisec, cmdline::parser &parser);
    static cmdline::parser getParser();
};

}
}
