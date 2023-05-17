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
    std::string str;
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

  SuffixAutomaton() : states_(1, {{}, 0, kBeforeRoot, ""}) {}

  // добавляем по символу и перестраиваем автомат
  void AddChar(char symbol) {
    ++total_len_;
    // создадим класс соответствующий [Sc]
    std::string str = states_[last_state_].str;
    int64_t curr = MakeState(states_[last_state_].len + 1, 0, str + symbol);

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
        str = states_[p].str + symbol;
        int64_t clone_id = MakeState(states_[p].len + 1,  // s0 - лонгест
                                     states_[q].suffix_link,  // суфлинк
                                     str + symbol);
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

  void GetOccurences(std::vector<std::pair<size_t, size_t>>& occurences) {
    occurences.resize(states_.size());
    std::vector<bool> used(states_.size());
    GetOccurencesFrom(0, occurences, used);
  }

  size_t CountGoodSubstrings(
      std::vector<std::pair<size_t, size_t>>& occurences) {
    size_t counter = 0;
    std::vector<bool> used(states_.size());
    CountGoodSubstringsFrom(0, occurences, used, counter);
    return counter;
  }

 private:
  int32_t MakeState(int32_t len, int32_t suffix_link, std::string str) {
    int32_t id = states_.size();
    states_.push_back({{}, len, suffix_link, str});
    return id;
  }
  void SetTerminalsFrom(size_t node) {
    if (node == 0) {
      return;
    }
    is_terminal_[node] = true;
    SetTerminalsFrom(states_[node].suffix_link);
  }

  void GetOccurencesFrom(size_t node,
                         std::vector<std::pair<size_t, size_t>>& occurences,
                         std::vector<bool>& used) {
    used[node] = true;
    if (is_terminal_[node]) {
      occurences[node].first = 0;
      occurences[node].second = 0;
    } else {
      occurences[node].first = 0;
      occurences[node].second = total_len_;
    }

    for (auto to : states_[node].go) {
      if (!used[to.second]) {
        GetOccurencesFrom(to.second, occurences, used);
      }
      // first occurence
      occurences[node].first =
          std::max(occurences[node].first, occurences[to.second].first + 1);
      // last occurence
      occurences[node].second =
          std::min(occurences[node].second, occurences[to.second].second + 1);
    }
  }

  void CountGoodSubstringsFrom(
      size_t node, std::vector<std::pair<size_t, size_t>>& occurences,
      std::vector<bool>& used, size_t& counter) {
    used[node] = true;
    int32_t first = occurences[node].first;
    int32_t second = occurences[node].second;
    if (node != 0 && first - second > states_[states_[node].suffix_link].len) {
      counter += std::min(first - second, states_[node].len) -
                 states_[states_[node].suffix_link].len;
      // std::cout << states_[node].str << ' ' << first << '\n';
      //++counter;
    }
    for (auto to : states_[node].go) {
      if (!used[to.second]) {
        CountGoodSubstringsFrom(to.second, occurences, used, counter);
      }
    }
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
  std::vector<std::pair<size_t, size_t>> occurences;
  // find first and last occurence for each node
  automaton.GetOccurences(occurences);
  // count good substrings
  auto ans = automaton.CountGoodSubstrings(occurences);
  std::cout << ans;
}

int main() {
  FastIO();
  std::string str;
  std::cin >> str;
  SuffixAutomaton automaton;
  automaton.ResetState();
  for (auto ch : str) {
    // std::cin >> ch;
    automaton.AddChar(ch);
  }
  Solve(automaton);
  return 0;
}