#pragma once

namespace procon26 {

template<typename T, int N>
struct Square {
  typedef T cell_type;
  typedef Square<T, N> this_type;

  static const int SIZE = N;
  cell_type data[N][N];
  short zk;

  Square();
  void initialize();
  void copy(const this_type& square);
  void reverse();
  void rotate();
  cell_type* operator[](int index);
  template<typename ConstIterator>
  ConstIterator load(ConstIterator iter);
  inline bool inBounds(int x, int y) const;
  inline cell_type at(int x, int y) const;
};

}

#include "details/Square.cpp"
