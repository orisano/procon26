
template<typename BoardType>
struct ExtendBoard : BoardType {
  using Derived = BoardType;

  ExtendBoard() : Derived(), eval(-1) {
    used_[0] = used_[1] = used_[2] = used_[3] = 0;
  }

  ExtendBoard(Derived b) : Derived(b), eval(-1) {
    used_[0] = used_[1] = used_[2] = used_[3] = 0;
  }

  bool isUsed(int id) const {
    assert(0 <= id && id < 256);
    return (used_[id >> 6] >> (id & 63)) & 1;
  }

  void useTile(int id) {
    assert(!isUsed(id));
    used_[id >> 6] |= 1ull << (id & 63);
  }

  int eval;
 private:
  std::uint64_t used_[4];
};

