#ifndef INCLUDE_HOGPQ_HPP
#define INCLUDE_HOGPQ_HPP

#include <vector>
#include <queue>
#include <functional>

namespace orliv {

template <typename T>
struct HogPQ {
  const T& min() {
    refreshq(miniq, delminiq);
    return miniq.top();
  }
  const T& max() {
    refreshq(maxiq, delmaxiq);
    return maxiq.top();
  }
  void emplace(const T& x) {
    miniq.emplace(x);
    maxiq.emplace(x);
    count++;
  }
  void minpop() {
    refreshq(miniq, delminiq);
    delmaxiq.emplace(miniq.top());
    miniq.pop();
    refreshq(miniq, delminiq);
    count--;
  }
  void maxpop() {
    refreshq(maxiq, delmaxiq);
    delminiq.emplace(maxiq.top());
    maxiq.pop();
    refreshq(maxiq, delmaxiq);
    count--;
  }
  std::size_t size() const { return count; }
  bool empty() const { return !count; }

 private:
  template <typename Q>
  inline void refreshq(Q& a, Q& b) const {
    while (!a.empty() && !b.empty() && a.top() == b.top()) {
      a.pop();
      b.pop();
    }
  }

  int count = 0;
  std::priority_queue<T, std::vector<T>, std::less<T>> maxiq, delmaxiq;
  std::priority_queue<T, std::vector<T>, std::greater<T>> miniq, delminiq;
};
}

#endif
