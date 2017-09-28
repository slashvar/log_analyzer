/*
 * time_tree.hh: time oriented tree
 * some hybrid construction between prefix tree and B+ tree
 * First level is dispatched against year, then month, day, hour, minute and
 * finally seconds. Last level (seconds) contains a containers.
 * Last level is fully linked to avoid traversal for range query.
 */

#ifndef _ALGOLIA_INTERVIEW_TIME_TREE_HH_
#define _ALGOLIA_INTERVIEW_TIME_TREE_HH_

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <unordered_map>
#include <vector>

static const size_t POOL_SIZE = 1 << 20;

template <typename CONTENT>
struct mem_pool {
  mem_pool() : cur_size(0) {
    new_pool();
  }

  mem_pool(mem_pool& p) = delete;

  ~mem_pool() {
    for (CONTENT* ptr : pools)
      free(ptr);
  }

  void new_pool() {
    CONTENT* ptr = (CONTENT*)calloc(POOL_SIZE, sizeof(CONTENT));
    pools.push_back(ptr);
    cur_size = 0;
  }

  CONTENT* alloc() {
    if (cur_size == POOL_SIZE) {
      new_pool();
    }
    CONTENT* ptr = pools.back() + cur_size;
    cur_size++;
    return ptr;
  }

  std::vector<CONTENT*> pools;
  size_t cur_size;
};

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

template <typename CONTENT>
struct time_tree {
  struct node {
    size_t rank(date d) {
      static auto cmp = [](const date d1, const date d2) { return d1.value < d2.value; };
      auto target = std::lower_bound(begin(keys), end(keys), d, cmp);
      return std::distance(begin(keys), target);
    }
    std::vector<date> keys;
    std::vector<node*> children;
    node *next, *prev;
    std::vector<CONTENT> store;
  };

  time_tree() {
    pool = std::make_shared<mem_pool<node>>();
    root = pool->alloc();
    date ld {{{0, 0, 0, 0, 0, 0}}};
    date rd {{{9999, 12, 31, 23, 59, 59}}};
    auto cur = root;
    for (int i = 0; i < 5; i++) {
      cur->keys.push_back(ld);
      cur->children.push_back(pool->alloc());
      cur = cur->children.back();
    }
    cur->keys.push_back(ld);
    left_sentinel = cur;
    cur = root;
    for (int i = 0; i < 5; i++) {
      cur->keys.push_back(rd);
      cur->children.push_back(pool->alloc());
      cur = cur->children.back();
    }
    cur->keys.push_back(rd);
    right_sentinel = cur;
    left_sentinel->next = cur;
    cur->prev = left_sentinel;
  }

  node* _upper(const date& target, node* cur) {
    for (; cur->children.size() > 0; cur = cur->children[std::min(cur->rank(target), cur->children.size() - 1)])
      continue;
    return cur;
  }

  node* upper_bound(const date& target)
  {
    auto cur = _upper(target, root);
    return cur;
  }

  void insert(const date& d, CONTENT x) {
    auto cur = root;
    for (unsigned i = 0; i < 6; i++ ) {
      size_t rk = cur->rank(d);
      if (rk >= cur->keys.size() && cur->keys[rk-1].split[i] == d.split[i])
        rk -= 1;
      else if (rk >= cur->keys.size() || cur->keys[rk].split[i] != d.split[i]) {
        auto prev = _upper(d, cur);
        if (prev->prev)
          prev = prev->prev;
        for (unsigned j = i; j < 6; j++) {
          cur->keys.insert(begin(cur->keys) + rk, d);
          cur = *cur->children.insert(begin(cur->children) + rk, pool->alloc());
          rk = 0;
        }
        cur->keys.push_back(d);
        cur->store.push_back(x);
        cur->prev = prev;
        cur->next = prev->next;
        prev->next = cur;
        if (cur->next)
          cur->next->prev = cur;
        return;
      }
      cur = cur->children[std::min(rk, cur->children.size() - 1)];
    }
    cur->store.push_back(x);
  }

  size_t count(const date& start, const date& end_) {
    auto begin = upper_bound(start);
    auto end = &*upper_bound(end_);
    size_t count = 0;
    for (auto cur = begin; cur && cur != end; cur = cur->next) {
      count += cur->store.size();
    }
    return count;
  }

  std::unordered_map<CONTENT, size_t> compute_freq(const date& start, const date& end_) {
    auto begin = &*upper_bound(start);
    auto end = &*upper_bound(end_);
    std::unordered_map<CONTENT, size_t> hist;
    for (auto cur = begin; cur && cur != end; cur = cur->next) {
      for (auto q : cur->store)
        hist[q] += 1;
    }
    return hist;
  }

  node* root;
  node *left_sentinel, *right_sentinel;
  std::shared_ptr<mem_pool<node>> pool;

};

#endif
