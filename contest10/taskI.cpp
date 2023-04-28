#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

class Trie {
 public:
  // static constexpr int32_t Null = -1;
  static constexpr int32_t Null = 1e6 + 100;

  struct Node {
    Node() = default;
    Node(int32_t parent, char symbol) : parent(parent), symbol(symbol) {}

    bool HasChild(char symbol) {
      return children.find(symbol) != children.end();
    }

    std::unordered_map<char, int32_t> children;
    int32_t parent{Null};
    char symbol{};
    bool is_terminal{false};
  };

  Trie() : nodes_(1) {}

  int32_t Add(const std::string& key) {
    int32_t now = 0;
    for (auto symbol : key) {
      if (!nodes_[now].HasChild(symbol)) {
        nodes_.emplace_back(now, symbol);
        nodes_[now].children[symbol] = nodes_.size() - 1;
      }
      now = nodes_[now].children[symbol];
    }
    nodes_[now].is_terminal = true;
    return now;
  }

  Node& Get(int32_t index) { return nodes_.at(index); }

  size_t NodeCount() { return nodes_.size(); }

 private:
  std::vector<Node> nodes_;
};

class AhoKorasikAutomaton {
 public:
  AhoKorasikAutomaton(const std::vector<std::string>& patterns)
      : patterns_(patterns) {
    state_ = 0;
    pattern_mapper_.resize(kMaxIdx, 0);
    int32_t i = 0;
    for (auto pattern : patterns) {
      pattern_mapper_[trie_.Add(pattern)] = i++;
    }
    suffix_links_.assign(trie_.NodeCount(), Trie::Null);
    terminal_links_.assign(trie_.NodeCount(), Trie::Null);
  }

  std::vector<std::string> MakeStep(char symbol) {
    state_ = CalcStep(state_, symbol);
    return ExtractMatches();
  }

  void ResetState() { state_ = 0; }

  void DFS(int32_t node, std::vector<uint8_t>& color, bool& ans) {
    if (trie_.Get(node).is_terminal || ans || GetTerminalLink(node) != 0) {
      return;
    }

    color[node] = 1;
    // bool ans;
    for (auto next : {'0', '1'}) {
      int32_t new_node = CalcStep(node, next);
      if (color[new_node] == 1) {
        ans = true;
        return;
      }
      if (color[new_node] == 0) {
        DFS(new_node, color, ans);
      }
    }
    color[node] = 2;
  }

 private:
  static constexpr size_t kMaxIdx = 2e6;
  std::vector<std::string> ExtractMatches() {
    std::vector<std::string> ans;
    size_t v = state_;
    while (v != 0) {
      auto& node = trie_.Get(v);
      if (node.is_terminal) {
        ans.push_back(patterns_[pattern_mapper_[v]]);
      }
      v = GetTerminalLink(v);
    }
    return ans;
  }

  int32_t CalcStep(int32_t node, char symbol) {
    auto& v = trie_.Get(node);
    if (v.HasChild(symbol)) {
      return v.children[symbol];
    }
    if (node == 0) {
      return 0;
    }
    return CalcStep(GetSuffixLink(node), symbol);
  }

  int32_t GetTerminalLink(int32_t node) {
    if (terminal_links_[node] != Trie::Null) {
      return terminal_links_[node];
    }
    if (node == 0 || trie_.Get(node).parent == 0) {
      terminal_links_[node] = 0;
      return 0;
    }
    int32_t v = GetSuffixLink(node);
    while (v != 0 && !trie_.Get(v).is_terminal) {
      if (terminal_links_[v] != Trie::Null) {
        v = terminal_links_[v];
      } else {
        v = GetSuffixLink(v);
      }
    }
    terminal_links_[node] = v;
    return v;
  }

  int32_t GetSuffixLink(int32_t node) {
    if (suffix_links_[node] != Trie::Null) {
      return suffix_links_[node];
    }
    if (node == 0 || trie_.Get(node).parent == 0) {
      suffix_links_[node] = 0;
      return 0;
    }
    auto& v = trie_.Get(node);
    suffix_links_[node] = CalcStep(GetSuffixLink(v.parent), v.symbol);
    return suffix_links_[node];
  }

  std::vector<int32_t> suffix_links_;
  std::vector<int32_t> terminal_links_;

  // std::unordered_map<int32_t, size_t> pattern_mapper_;
  std::vector<size_t> pattern_mapper_;

  int32_t state_{0};
  Trie trie_;
  std::vector<std::string> patterns_;
};

const size_t kMaxLen = 1e6 + 1;

void Solve(std::vector<std::string>& patterns) {
  auto aho = AhoKorasikAutomaton(patterns);
  std::vector<uint8_t> visited(kMaxLen, 0);
  bool answer = false;
  aho.DFS(0, visited, answer);
  std::cout << (answer ? "TAK" : "NIE");
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

int main() {
  FastIO();
  size_t patterns_count;
  std::cin >> patterns_count;
  std::vector<std::string> patterns(patterns_count);
  for (size_t i = 0; i < patterns_count; ++i) {
    std::cin >> patterns[i];
  }
  Solve(patterns);
  return 0;
}