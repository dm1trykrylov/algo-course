// Knuth–Morris–Pratt algorithm
#include <iostream>
#include <string>
#include <vector>

template <typename T>
void Print(std::vector<T>& data, char delim = '\n') {
  for (auto element : data) {
    std::cout << element << delim;
  }
}

const size_t kMaxLength = 1e6 + 2;
std::vector<int16_t> prefix(kMaxLength, 0);

size_t PrefixFunction(std::string& str) {
  size_t current_pref_pos = 0;
  for (size_t i = 1; i < str.length(); ++i) {
    current_pref_pos = prefix[i - 1];
    while (current_pref_pos > 0 && str[i] != str[current_pref_pos]) {
      current_pref_pos = prefix[current_pref_pos - 1];
    }
    if (str[i] == str[current_pref_pos]) {
      ++current_pref_pos;
    }
    prefix[i] = current_pref_pos;
  }
  return prefix[str.length() - 1];
}

void Solve(size_t count) {
  std::vector<std::string> strings(count);
  std::string super_string;
  std::string test_string;
  for (size_t i = 0; i < count; ++i) {
    std::cin >> strings[i];
  }
  super_string = strings[0];
  for (size_t i = 1; i < count; ++i) {
    size_t offset = 0;
    if (super_string.length() > strings[i].length()) {
      offset = super_string.length() - strings[i].length();
    }
    test_string = strings[i] + '#' + super_string.substr(offset);
    size_t max_prefix = PrefixFunction(test_string);
    super_string += strings[i].substr(max_prefix);
  }
  std::cout << super_string;
}

void SetFastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

int main() {
  SetFastIO();
  size_t strings_count;
  std::cin >> strings_count;
  Solve(strings_count);
  return 0;
}