#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

struct Query {
  int64_t value;
  char type;
};

void CompressValues(std::vector<Query>& queries, std::vector<int64_t>& data) {
  std::vector<int64_t> tmp(data);
  std::sort(tmp.begin(), tmp.end());
  auto last = std::unique(tmp.begin(), tmp.end());
  tmp.erase(last, tmp.end());
  for (auto& q : queries) {
    q.value =
        std::lower_bound(tmp.begin(), tmp.end(), q.value) - tmp.begin() + 1;
  }
}

void ReadQueries(size_t& n, std::vector<Query>& queries) {
  std::cin >> n;
  queries.resize(n);
  std::vector<int64_t> values;
  for (size_t i = 0; i < n; ++i) {
    std::cin >> queries[i].type >> queries[i].value;
    values.push_back(queries[i].value);
  }
  std::sort(values.begin(), values.end());
  CompressValues(queries, values);
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

void Insert(std::vector<int64_t>& destination, int64_t value) {
  ++destination[value];
}

void Erase(std::vector<int64_t>& destination, int64_t value) {
  destination[value] = 0;
}

bool Get(std::vector<int64_t>& destination, int64_t value) {
  return destination[value] > 0;
}

int main() {
  // FastIO();
  size_t n;
  std::vector<Query> queries;
  ReadQueries(n, queries);
  std::vector<int64_t> data(n + 1);
  for (auto query : queries) {
    if (query.type == '+') {
      Insert(data, query.value);
    }
    if (query.type == '-') {
      Erase(data, query.value);
    }
    if (query.type == '?') {
      std::cout << (Get(data, query.value) ? "YES" : "NO") << '\n';
    }
  }
  return 0;
}