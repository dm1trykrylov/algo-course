#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class SuffixAutomaton {
  static constexpr int32_t kBeforeRoot = -1;

  struct State {
    std::unordered_map<char, int32_t> go;  // переходики
    int32_t len;                           // размер longest
    int32_t suffix_link;  // суфлинк ведет в класс где лежит самый большой
                          // суффикс longest-а, который лежит в другом классе
                          // (класс эквивалентности состоит из longest и
                          // нескольких его самых длинных суффиксов)
    int32_t parent;
    char ch;
  };

 public:
  void ResetState() { use_state_ = 0; }

  bool Step(char symbol) {
    if (states_[use_state_].go.contains(symbol)) {
      use_state_ = states_[use_state_].go[symbol];
      return true;
    }
    return false;
  }

  SuffixAutomaton() : states_(1, {{}, 0, kBeforeRoot, 0, '\0'}) {}

  // добавляем по символу и перестраиваем автомат
  void AddChar(char symbol) {
    ++total_len_;
    // создадим класс соответствующий [Sc]
    // std::string str = states_[last_state_].str;
    int64_t curr =
        MakeState(states_[last_state_].len + 1, 0, last_state_, symbol);

    int64_t p = last_state_;
    // пока нет переходов добавляем новые переходы в класс [Sc]
    while (p != kBeforeRoot && !states_[p].go.contains(symbol)) {
      states_[p].go[symbol] = curr;
      p = states_[p].suffix_link;
    }

    if (p == kBeforeRoot) {
      // случай когда мы ушли за корень - символ был в нашей строке впервые
      states_[curr].suffix_link = 0;
    } else {
      int64_t q = states_[p].go[symbol];
      if (states_[p].len + 1 == states_[q].len) {
        // случай когда не появляется новых классов
        states_[curr].suffix_link = q;
      } else {
        // str = states_[p].str + symbol;
        int64_t clone_id = MakeState(states_[p].len + 1,  // s0 - лонгест
                                     states_[q].suffix_link,  // суфлинк
                                     p, symbol);
        states_[clone_id].go = states_[q].go;
        // нужно перенаправить переходы в клона
        while (p != kBeforeRoot && states_[p].go[symbol] == q) {
          states_[p].go[symbol] = clone_id;
          p = states_[p].suffix_link;
        }
        states_[q].suffix_link = states_[curr].suffix_link = clone_id;
      }
    }

    last_state_ = curr;  // longest класса [Sc]
  }

  void SetTerminals() {
    is_terminal_.resize(states_.size());
    SetTerminalsFrom(last_state_);
  }

  void CountPathsToTerminal(std::vector<size_t>& count) {
    count.resize(states_.size());
    std::vector<bool> used(states_.size(), false);
    CountPathsToTerminalFrom(0, count, used);
  }

  std::pair<size_t, std::string> FindMaxRepeatingString(
      std::vector<size_t>& count) {
    std::vector<bool> used(count.size());
    auto res = FindMaxRepeatingStringFrom(0, count, used);
    int32_t current = res.second;
    std::string str;
    while (current != 0) {
      str += states_[current].ch;
      current = states_[current].parent;
    }
    std::reverse(str.begin(), str.end());
    return {res.first, str};
  }

 private:
  int32_t MakeState(int32_t len, int32_t suffix_link, int32_t parent, char ch) {
    int32_t id = states_.size();
    states_.push_back({{}, len, suffix_link, parent, ch});
    return id;
  }
  void SetTerminalsFrom(size_t node) {
    if (node == 0) {
      return;
    }
    is_terminal_[node] = true;
    SetTerminalsFrom(states_[node].suffix_link);
  }
  void CountPathsToTerminalFrom(size_t node, std::vector<size_t>& count,
                                std::vector<bool>& used) {
    used[node] = true;
    if (is_terminal_[node]) {
      count[node] = 1;
    }
    for (auto to : states_[node].go) {
      if (!used[to.second]) {
        CountPathsToTerminalFrom(to.second, count, used);
      }
      count[node] += count[to.second];
    }
  }
  std::pair<size_t, int32_t> FindMaxRepeatingStringFrom(
      size_t node, const std::vector<size_t>& count, std::vector<bool>& used) {
    used[node] = true;
    size_t current_idx = states_[node].len * count[node];
    int32_t current_state = node;
    for (auto to : states_[node].go) {
      if (!used[to.second]) {
        auto res = FindMaxRepeatingStringFrom(to.second, count, used);
        if (res.first > current_idx) {
          current_idx = res.first;
          current_state = res.second;
        }
      }
    }
    return {current_idx, current_state};
  }
  int32_t last_state_{0};
  std::vector<State> states_;
  std::vector<bool> is_terminal_;
  int32_t use_state_{0};
  size_t total_len_{0};
};

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

void Solve(SuffixAutomaton& automaton) {
  automaton.SetTerminals();
  // get number of occurences for each string
  std::vector<size_t> count;
  automaton.CountPathsToTerminal(count);
  auto ans = automaton.FindMaxRepeatingString(count);
  std::cout << ans.first << '\n' << ans.second.length() << '\n';
  for (auto ch : ans.second) {
    std::cout << ch - 'a' << ' ';
  }
}

int main() {
  FastIO();
  size_t len;
  size_t max_digit;
  std::cin >> len >> max_digit;
  SuffixAutomaton automaton;
  size_t ch;
  automaton.ResetState();
  for (size_t i = 0; i < len; ++i) {
    std::cin >> ch;
    automaton.AddChar('a' + ch);
  }
  Solve(automaton);
  return 0;
}