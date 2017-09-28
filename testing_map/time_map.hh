#ifndef TIME_MAP_HH_
#define TIME_MAP_HH_

#include <algorithm>
#include <array>
#include <cassert>
#include <memory>
#include <unordered_map>
#include <map>
#include <vector>

struct date {
  date(const std::array<unsigned, 6>& s) : split(s) {
    value = 0;
    for (unsigned x : split) {
      value = value * 100 + x;
    }
  }
  date() : value(0) {}
  std::uint64_t value;
  std::array<unsigned, 6> split;
};


struct date_cmp {
  bool operator() (const date& d1, const date& d2)
  {
    return d1.value < d2.value;
  }
};

template <typename CONTENT>
struct time_tree {
  time_tree() {
    date ld {{{0, 0, 0, 0, 0, 0}}};
    date rd {{{9999, 12, 31, 23, 59, 59}}};
    root.insert({ld, {}});
    root.insert({rd, {}});
  }

  void insert(const date& d, CONTENT x) {
    root[d].push_back(x);
  }

  size_t count(const date& dstart, const date& dstop) {
    auto start = root.lower_bound(dstart);
    auto stop = root.upper_bound(dstop);
    size_t count = 0;
    for (; start != stop; start++)
      count += start->second.size();
    return count;
  }

  std::unordered_map<CONTENT, size_t> compute_freq(const date& dstart, const date& dstop) {
    auto start = root.lower_bound(dstart);
    auto stop = root.upper_bound(dstop);
    std::unordered_map<CONTENT, size_t> hist;
    for (; start != stop; start++) {
      for (auto q : start->second)
        hist[q] += 1;
    }
    return hist;

  }

  std::map<date, std::vector<CONTENT>, date_cmp> root;
};

#endif /* TIME_MAP_HH_ */
