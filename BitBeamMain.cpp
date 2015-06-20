#include "Home.hpp"
#include "takumi/BitBeam.hpp"
#include <string>
#include <iostream>

using namespace procon26;
using namespace std;

int main(int argc, const char** argv)
{
  Home home = Home();
  string filepath;
  
  if (argc > 1) filepath = string(argv[1]);
  else cout << "filepath:", cin >> filepath;
  home.load(filepath);

  takumi::BitBeam solver;
  solver.solve(home, 10000);

  return 0;
}
