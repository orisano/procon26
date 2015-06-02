#pragma once
#include "Action.hpp"
#include <deque>
#include <string>
#include <fstream>

namespace procon26 {

struct Answer {
  std::deque<Action> log;
  void push(Action action) { log.emplace_back(action);  };
  void pop() { log.pop_back(); };
  void save(const std::string& filepath)
  {
    std::sort(log.begin(), log.end(), [](const Action& a, const Action& b) { return a.id < b.id; });

    std::ofstream ofs(filepath);
    for (const auto& action : log) {
      ofs << action.id << " " << action.x << " " << action.y << " " << action.rotate << " " << action.inverse << std::endl;
    }
  }
};

}
