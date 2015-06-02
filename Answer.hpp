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
  void save(const std::string& filepath, int tile_count)
  {
    std::sort(log.begin(), log.end(), [](const Action& a, const Action& b) { return a.id < b.id; });
    log.emplace_back(Action(-1));

    std::ofstream ofs(filepath);
    auto iter = log.begin();
    for (auto i = 0; i < tile_count; i++){
      if (iter->id == i){
        ofs << iter->x << ' ' << iter->y << ' ' << "HT"[iter->inverse] << ' ' << iter->rotate * 90;
        ++iter;
      }
      ofs << "\n";
    }
    ofs.close();
    log.pop_back();
  }
};

}
