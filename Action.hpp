#pragma once
namespace procon26 {

struct Action {
  Action() {}
  explicit Action(int id_, int x_=0, int y_=0, int rotate_=0, int inverse_=0) :
    id(id_), x(x_), y(y_), rotate(rotate_), inverse(inverse_) {}

  int id;
  int x;
  int y;
  int rotate;
  int inverse;
};

}