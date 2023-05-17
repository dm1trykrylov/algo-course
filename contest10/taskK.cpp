// 87094842
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class SuffixAutomaton {
  static constexpr int64_t kBeforeRoot = -1;

  struct State {
    std::unordered_map<char, int64_t> go;  // переходики
    int64_t len;                           // размер longest
    int64_t suffix_link;  // суфлинк ведет в класс где лежит самый большой
                          // суффикс longest-а, который лежит в другом классе
                          // (класс эквивалентности состоит из longest и
                          // нескольких его самых длинных суффиксов)
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

  SuffixAutomaton() : states_(1, {{}, 0, kBeforeRoot}) {}

  // добавляем по символу и перестраиваем автомат
  void AddChar(char symbol) {
    // создадим класс соответствующий [Sc]
    int64_t curr = MakeState(states_[last_state_].len + 1, 0);
    int64_t p_node = last_state_;  // пока нет переходов добавляем новые в [Sc]
    while (p_node != kBeforeRoot && !states_[p_node].go.contains(symbol)) {
      states_[p_node].go[symbol] = curr;
      p_node = states_[p_node].suffix_link;
    }
    if (p_node == kBeforeRoot) {
      // случай когда мы ушли за корень - символ был в нашей строке впервые
      states_[curr].suffix_link = 0;
    } else {
      int64_t q_node = states_[p_node].go[symbol];
      if (states_[p_node].len + 1 == states_[q_node].len) {
        // случай когда не появляется новых классов
        states_[curr].suffix_link = q_node;
      } else {
        int64_t clone_id = MakeState(states_[p_node].len + 1,  // s0 - лонгест
                                     states_[q_node].suffix_link  // суфлинк
        );
        states_[clone_id].go = states_[q_node].go;
        // нужно перенаправить переходы в клона
        while (p_node != kBeforeRoot && states_[p_node].go[symbol] == q_node) {
          states_[p_node].go[symbol] = clone_id;
          p_node = states_[p_node].suffix_link;
        }
        states_[q_node].suffix_link = states_[curr].suffix_link = clone_id;
      }
    }
    last_state_ = curr;  // longest класса [Sc]
  }

 private:
  int64_t MakeState(int64_t len, int64_t suffix_link) {
    int64_t state_id = states_.size();
    states_.push_back({{}, len, suffix_link});
    return state_id;
  }
  int64_t last_state_{0};
  std::vector<State> states_;

  int64_t use_state_{0};
};

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

void CheckOccurence(SuffixAutomaton& automaton, std::string& str) {
  automaton.ResetState();
  size_t cnt = 0;

  for (auto symbol : str) {
    if (automaton.Step(symbol)) {
      ++cnt;
    } else {
      break;
    }
  }
  // std::cout << cnt << std::endl;
  if (cnt == str.length()) {
    std::cout << "YES\n";
  } else {
    std::cout << "NO\n";
  }
}

void ProcessQuery(SuffixAutomaton& automaton, char query, std::string& str) {
  for (auto& symbol : str) {
    symbol = std::tolower(symbol);
  }
  if (query == '?') {
    CheckOccurence(automaton, str);
  } else {
    for (auto symbol : str) {
      automaton.AddChar(symbol);
    }
  }
}

int main() {
  FastIO();
  SuffixAutomaton automaton;
  char query;
  std::string str;
  while (std::cin >> query) {
    std::cin >> str;
    ProcessQuery(automaton, query, str);
  }
  return 0;
}