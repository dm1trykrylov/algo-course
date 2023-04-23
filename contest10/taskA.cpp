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

std::vector<size_t> PrefixFunction(std::string& str) {
  std::vector<size_t> prefix(str.length(), 0);
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
  return prefix;
}

std::vector<size_t> KMP(std::string text, std::string& pattern) {
  size_t text_len = text.length();
  size_t pattern_len = pattern.length();
  text = pattern + '#' + text;
  size_t total_len = text_len + pattern_len + 1;
  auto prefix = PrefixFunction(text);
  std::vector<size_t> occurences;
  for (size_t pos = 0; pos < text_len; ++pos) {
    if (prefix[pattern_len + 1 + pos] == pattern_len) {
      occurences.push_back(pos + 1 - pattern_len);
    }
  }
  return occurences;
}

void SetFastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

int main() {
  SetFastIO();
  std::string text;
  std::string pattern;
  std::cin >> text >> pattern;
  auto answer = KMP(text, pattern);
  Print(answer);
  return 0;
}