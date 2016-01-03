#ifndef INCLUDE_LEN_QUEUE_HPP
#define INCLUDE_LEN_QUEUE_HPP

#include <cassert>
#include "hogpq.hpp"

namespace orliv {

template <typename T>
struct LenQueue {
  typedef LenQueue<T> self_t;
  typedef HogPQ<T> queue_t;
  LenQueue(std::size_t cap) : cap(cap) {}
  void push(const T& x) {
    if (Q_.size() < cap) {
      Q_.emplace(x);
      return;
    }
    Q_.emplace(x);
    Q_.maxpop();
  }
  void pop() { Q_.minpop(); }
  T top() { return Q_.min(); }
  bool empty() { return Q_.empty(); }
  void meld(self_t& q) {
    if (Q_.size() < q.size()) swap(Q_, q);
    while (!q.empty()) push(q.top()), q.pop();
  }
  queue_t Q_;
  const std::size_t cap;
};
}

#endif
