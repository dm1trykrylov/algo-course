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
  size_t AddChar(char symbol) {
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
    return last_state_;
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

  // for each substring set a string to which it belongs
  void SetSubstringOrigins(size_t first_sep_id, size_t second_sep_id) {
    std::vector<bool> used(states_.size(), false);
    separators_.resize(states_.size());
    SetSubstringOriginsFrom(0, first_sep_id, second_sep_id,used);
  }

  std::string FindMaxRepeatingString(
      std::vector<size_t>& count, size_t k) {
    std::vector<bool> used(count.size());
    size_t ans = 0;
    FindMaxRepeatingStringFrom(0, count, used, 0, k, ans);
    int32_t current = ans;
    std::string str;
    while (current != 0) {
      str += states_[current].ch;
      current = states_[current].parent;
    }
    std::reverse(str.begin(), str.end());
    return str;
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

  void SetSubstringOriginsFrom(size_t node, size_t first_id, size_t second_id, std::vector<bool>& used) {
    used[node] = true;
    if (node == first_id) {
      separators_[node] = 1;
    } 
    if (node == second_id) {
      separators_[node] = 2;
    }
    for (auto to : states_[node].go) {
      if (!used[to.second]) {
        SetSubstringOriginsFrom(to.second, first_id, second_id, used);
      }
      separators_[node] |= separators_[to.second];
    }
  }

  void FindMaxRepeatingStringFrom(size_t node, const std::vector<size_t>& count,
                                  std::vector<bool>& used, size_t curr_cnt,
                                  size_t k, size_t& ans) {
    used[node] = true;
    curr_cnt += count[node];
    if (count[node] == k && node != 0) {
      ans = node;
      return;
    }
    for (auto to : states_[node].go) {
      if (!used[to.second]) {
        if (ans == 0) {
          FindMaxRepeatingStringFrom(to.second, count, used, curr_cnt, k, ans);
        }
      }
    }
  }
  int32_t last_state_{0};
  std::vector<State> states_;
  std::vector<uint8_t> separators_;
  std::vector<bool> is_terminal_;
  int32_t use_state_{0};
  size_t total_len_{0};
};

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

void Solve(SuffixAutomaton& automaton) {
  std::string first;
  std::string second;
  std::cin >> first >> second;
  size_t first_str_size = first.length();
  first = first + '$' + second + '*';
  size_t state_id;
  size_t first_id;
  size_t second_id;
  for (auto ch : first) {
    state_id = automaton.AddChar(ch);
    if (ch == '$') {
      first_id = state_id;
    }
    if (ch == '*') {
      second_id = state_id;
    }
  }
  automaton.SetTerminals();
  // get number of occurences for each string
  std::vector<size_t> count;
  //automaton.CountPathsToTerminal(count);
  automaton.SetSubstringOrigins(first_id, second_id);
  size_t k = 0;
  std::cin >> k;
  auto ans = automaton.FindMaxRepeatingString(count, k);
  std::cout << ans << '\n';
}

int main() {
  FastIO();
  SuffixAutomaton automaton;
  automaton.ResetState();
  Solve(automaton);
  return 0;
}