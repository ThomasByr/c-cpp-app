
#ifndef __tests_ctpl_test_H__
#define __tests_ctpl_test_H__

#include <chrono>
#include <vector>

#include "ctpl.hpp"
#include "m.h"

namespace ctpl_test {

namespace detail {

unsigned task(unsigned id) {
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s);
  return id;
}

void ctpl_test_0() {
  using namespace std::chrono;
  const unsigned n_tasks = 8;

  // launch n_tasks tasks in parallel using n_tasks threads
  // so that the test is not too slow
  ctpl::thread_pool p{n_tasks};
  std::vector<std::future<unsigned>> futures{n_tasks};

  // initialize chronometer to test duration
  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  for (unsigned i = 0; i < n_tasks; ++i) {
    futures[i] = p.push([i](unsigned) { return task(i); });
  }
  for (unsigned i = 0; i < n_tasks; ++i) { assert_eq(futures[i].get(), i); }

  // stop chronometer
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  // compute duration
  auto duration = duration_cast<milliseconds>(t2 - t1).count();
  // check that the duration is around 1 second
  assert_leq(std::abs(duration - 1000), 100);
}

void ctpl_test_1() {
  std::vector<int> v;
  std::vector<int> w;
  std::vector<std::future<int>> futures;
  const int n = 1000;

  ctpl::thread_pool p{4};
  for (int i = 0; i < n; ++i) {
    futures.push_back(p.push([i, &v](int) {
      v.push_back(i);
      return i;
    }));
  }
  for (auto &f : futures) { w.push_back(f.get()); }
  assert_eq(v.size(), n);
  assert_eq(futures.size(), n);

  std::sort(v.begin(), v.end());
  for (int i = 0; i < n; ++i) {
    assert_eq(v[i], i);
    assert_eq(w[i], i);
  }
}

} // namespace detail

void ctpl_test() {
  using namespace detail;
  test_case(ctpl_test_0);
  test_case(ctpl_test_1);
}

} // namespace ctpl_test

#endif // __tests_ctpl_test_H__
