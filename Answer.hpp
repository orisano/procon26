#pragma once
#include "Action.hpp"
#include <deque>
#include <string>

namespace procon26 {

struct Answer {
  void push(Action action);
  void pop();
  void clear();
  Action latestAction();
  const std::deque<Action>& read() const;
  void save(const std::string& filepath, int tile_count);
  void load(const std::string& filepath);
private:
  std::deque<Action> log_;
};

}
