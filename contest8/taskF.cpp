#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;
const int length = 1e5 + 5, logn = 20;
int m, n, k, t, binup[length][logn + 1], depth[length], p[length], dfsTimer,
    timeIn[length], timeOut[length], ans, v1, v2;
pair<int, int> e[length];
bool vis[length], fl = false;
int LCA(int v, int u);
void dfsEdges(int v = 0, int pr = 0);
void DFSPrecalc(int v, int pt = 0);
bool CheckParent(int a = 0, int b = 0);
vector<vector<int> > adjacency_list;
int main() {
  cin.tie(0);
  cin >> n;
  adjacency_list.resize(n);
  for (int i = 0; i < n - 1; ++i) {
    cin >> v1 >> v2;
    --v1;
    --v2;
    adjacency_list[v1].push_back(v2);
    adjacency_list[v2].push_back(v1);
  }
  DFSPrecalc(0);
  cin >> m;
  while (m--) {
    cin >> v1 >> v2;
    --v1;
    --v2;
    if (CheckParent(v1, v2) || CheckParent(v2, v1))
      cout << abs(depth[v1] - depth[v2]) << '\n';
    else
      cout << depth[v1] + depth[v2] - 2 * depth[LCA(v1, v2)] << '\n';
  }
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
int LCA(int v, int u) {
  int ans;
  if (depth[v] > depth[u]) {
    swap(u, v);
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
