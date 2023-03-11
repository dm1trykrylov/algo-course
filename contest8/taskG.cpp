#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const int length = 1e5 + 5, logn = 20;
int k, t, binup[length][logn + 1], depth[length], p[length], dfsTimer,
    timeIn[length], timeOut[length], ans;

int LCA(size_t v, size_t u);
void DFSPrecalc(int v, int pt = 0);
bool CheckParent(int a = 0, int b = 0);
std::vector<std::vector<size_t> > adjacency_list;
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  size_t vertex_count, queries_count;
  std::cin >> vertex_count >> queries_count;
  adjacency_list.resize(vertex_count);
  size_t parent;
  for (size_t i = 1; i < vertex_count; ++i) {
    std::cin >> parent;
    adjacency_list[i].push_back(parent);
    adjacency_list[parent].push_back(i);
  }
  DFSPrecalc(0);
  size_t a_1, a_2, v = 0;
  std::cin >> a_1 >> a_2;
  size_t x, y, z;
  std::cin >> x >> y >> z;
  size_t sum_answers = 0;
  while (queries_count--) {
    v = LCA((a_1 + v) % vertex_count, a_2);
    a_1 = (x * a_1 + y * a_2 + z) % vertex_count;
    a_2 = (x * a_2 + y * a_1 + z) % vertex_count;
    sum_answers += v;
  }
  std::cout << sum_answers;
  return 0;
}
//
void DFSPrecalc(int v, int pt) {
  timeIn[v] = ++dfsTimer;
  binup[v][0] = pt;
  p[v] = pt;
  depth[v] = depth[p[v]] + 1;
  for (int i = 1; i <= logn; ++i) {
    binup[v][i] = binup[binup[v][i - 1]][i - 1];
  }
  for (auto it = adjacency_list[v].begin(); it != adjacency_list[v].end();
       ++it) {
    int to = *it;
    if (to != pt) DFSPrecalc(to, v);
  }
  timeOut[v] = ++dfsTimer;
}
//
bool CheckParent(int a, int b) {
  return (timeIn[a] <= timeIn[b] && timeOut[a] >= timeOut[b]);
}
//
int LCA(size_t v, size_t u) {
  size_t ans;
  if (depth[v] > depth[u]) {
    std::swap(u, v);
  }
  if (CheckParent(u, v)) {
    ans = u;
  } else if (CheckParent(v, u)) {
    ans = v;
  } else {
    for (int i = logn; i >= 0; --i)
      if (!CheckParent(binup[v][i], u)) {
        v = binup[v][i];
      } else {
        ans = binup[v][i];
      }
  }
  return ans;
}
