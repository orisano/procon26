#include "Home.hpp"
#include "takumi/BitBeam.hpp"
#include <string>
#include <iostream>

using namespace procon26;
using namespace std;

int main(int argc, char** argv) {
    Home home = Home();
    takumi::BitBeam solver;

    cmdline::parser parser = takumi::BitBeam::getParser();
    parser.add<string>("filepath", 'f', "quest filepath", true);
    parser.add<int>("time", 't', "time", false, 10000);

    parser.parse_check(argc, argv);

    const string filepath = parser.get<string>("filepath");
    const int millisec = parser.get<int>("time");

    home.load(filepath);

    solver.solve(home, millisec, parser);

    return 0;
}