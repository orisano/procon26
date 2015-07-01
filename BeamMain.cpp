#include "Home.hpp"
#include "takumi/Beam.hpp"
#include <string>
#include <iostream>
#include "external/cmdline/cmdline.h"

using namespace procon26;
using namespace std;

int main(int argc, char** argv) {
    Home home = Home();
    takumi::Beam solver;

    cmdline::parser parser = takumi::Beam::getParser();
    parser.add<string>("filepath", 'f', "quest file path", true);
    parser.add<int>("time", 't', "time [ms]", false, 10000);

    parser.parse_check(argc, argv);

    const string filepath = parser.get<string>("filepath");
    const int millisec = parser.get<int>("time");

    home.load(filepath);
    solver.solve(home, millisec, parser);

    return 0;
}
