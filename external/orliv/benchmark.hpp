#ifndef INCLUDE_BENCHMARK_HPP
#define INCLUDE_BENCHMARK_HPP

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <iostream>

namespace orliv {
namespace misc {

struct __bench__ {
  typedef std::chrono::time_point<std::chrono::system_clock> time_type;
  time_type start;
  char msg[128];
  __bench__(const char* format, ...) __attribute__((format(printf, 2, 3))) {
    std::va_list args;
    va_start(args, format);
    std::vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    start = get_time();
  }
  ~__bench__() {
    time_type end = get_time();
    std::fprintf(stderr, "%s: %6d ms\n", msg, diff_time(start, end));
  }
  time_type get_time() { return time_type::clock::now(); }
  int diff_time(time_type a, time_type b) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count();
  }
  operator bool() { return false; }
};
}
}

#ifndef NO_BENCH
#define benchmark(...)                                                    \
  if (orliv::misc::__bench__ __b__ = orliv::misc::__bench__(__VA_ARGS__)) \
    ;                                                                     \
  else
#else
#define benchmark(...) if (true)
#endif

#endif
