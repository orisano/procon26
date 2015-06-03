#include "Answer.hpp"
#include <deque>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace procon26 {

void Answer::push(Action action)
{
  log_.emplace_back(action);
}

void Answer::pop()
{
  log_.pop_back();
}

void Answer::clear()
{
  log_.clear();
}

Action Answer::latestAction()
{
  return log_.back();
}

const std::deque<Action>& Answer::read() const
{
  return log_;
}

void Answer::save(const std::string& filepath, int tile_count)
{
  std::sort(log_.begin(), log_.end(), [](const Action& a, const Action& b) { return a.id < b.id; });

  log_.emplace_back(Action(-1));
  std::ofstream ofs(filepath);
  auto iter = log_.begin();
  for (auto i = 0; i < tile_count; i++) {
    if (iter->id == i) {
      ofs << iter->x << ' ' << iter->y << ' ' << "HT"[iter->inverse] << ' ' << iter->rotate * 90;
      ++iter;
    }
    ofs << "\n";
  }
  ofs.close();
  log_.pop_back();
}

void Answer::load(const std::string& filepath)
{
  clear();

  std::ifstream ifs(filepath);
  auto id = 0;
  for (std::string line; std::getline(ifs, line); ++id) {
    if (line == "") continue;
    std::stringstream ss(line);
      
    std::string sinv;
    int x, y, rot, inv;
    ss >> x >> y >> sinv >> rot;
    inv = sinv == "T";
    rot /= 90;
    push(Action(id, x, y, rot, inv));
  }
}

}