//{{{ uf.hpp
#ifndef INCLUDE_UF_HPP
#define INCLUDE_UF_HPP
#include <vector>
#include <algorithm>
namespace orliv {
//{{{ UF merge, same, root, size, count
struct UF {
  std::vector<int> data;
  int cnt;
  UF(int n) : data(n, -1), cnt(n) {}
  bool merge(int a, int b){
    a = root(a);
    b = root(b);
    if (a != b){
      cnt--;
      if (data[b] < data[a]) std::swap(a, b);
      data[a] += data[b];
      data[b] = a;
    }
    return a != b;
  }
  bool same(int a, int b){ return root(a) == root(b); }
  int root(int a){ return data[a] >= 0 ? data[a] = root(data[a]) : a; }
  int size(int a){ return -data[root(a)]; }
  int count(){ return cnt; }
};
//}}}
//{{{ UF2d merge, same, size, count
struct UF2d {
  UF uf;
  const int W, H;
  UF2d(int w, int h) : uf(w * h), W(w), H(h) {}
  bool merge(int ax, int ay, int bx, int by){ return uf.merge(ay * W + ax, by * W + bx); }
  bool same(int ax, int ay, int bx, int by){ return uf.same(ay * W + ax, by * W + bx); }
  int size(int x, int y){ return uf.size(y * W + x); }
  int root(int x, int y){ return uf.root(y * W + x); }
  int count(){ return uf.count(); }
};
//}}}
}
#endif
//}}}

