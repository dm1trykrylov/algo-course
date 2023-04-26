// Double Hashing
#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

template <typename T, typename U>
std::ostream& operator<<(std::ostream& stream, const std::pair<T, U>& pair) {
  stream << pair.first << ' ' << pair.second;
  return stream;
}

template <typename T>
void Print(const std::vector<T>& data, char delim = '\n') {
  for (auto element : data) {
    std::cout << element << delim;
  }
}

template <typename T>
void Read(size_t size, std::vector<T>& data) {
  for (size_t i = 0; i < size; ++i) {
    std::cin >> data[i];
  }
}

template <typename T>
struct std::hash<std::pair<T, T>> {
  T operator()(const std::pair<T, T>& pair) const noexcept {
    uintmax_t hash = std::hash<T>{}(pair.first);
    hash <<= sizeof(uintmax_t) * 4;
    hash ^= std::hash<T>{}(pair.second);
    return std::hash<uintmax_t>{}(hash);
  }
};

struct ModInt {
 private:
  static constexpr size_t kHashRate = 2;
  uint64_t numbers_[kHashRate];
  static constexpr uint64_t kMods[] = {2000049943, 0xFFFFFFFF};

 public:
  ModInt() : ModInt(0) {}
  ModInt(uint64_t number) {
    for (size_t i = 0; i < kHashRate; ++i) {
      numbers_[i] = number % kMods[i];
    }
  }

  void Mod(uint64_t& number, const uint64_t& mod) {
    if (number > mod) {
      number -= mod;
    }
  }

  ModInt& operator+=(ModInt other) {
    for (size_t i = 0; i < kHashRate; ++i) {
      numbers_[i] += other.numbers_[i];
      if (numbers_[i] > kMods[i]) {
        numbers_[i] -= kMods[i];
      }
    }
    return *this;
  }

  ModInt& operator-=(ModInt other) {
    for (size_t i = 0; i < kHashRate; ++i) {
      numbers_[i] += kMods[i];
      numbers_[i] -= other.numbers_[i];
      numbers_[i] %= kMods[i];
    }
    return *this;
  }
  ModInt& operator*=(ModInt other) {
    for (size_t i = 0; i < kHashRate; ++i) {
      numbers_[i] *= other.numbers_[i];
      numbers_[i] %= kMods[i];
    }
    return *this;
  }

  std::pair<uint64_t, uint64_t> Int() { return {numbers_[0], numbers_[1]}; }

  friend bool operator<(ModInt lhs, ModInt rhs);
  friend bool operator==(ModInt lhs, ModInt rhs);
};

ModInt operator+(ModInt lhs, ModInt rhs) {
  lhs += rhs;
  return lhs;
}

ModInt operator*(ModInt lhs, ModInt rhs) {
  lhs *= rhs;
  return lhs;
}

ModInt operator-(ModInt lhs, ModInt rhs) {
  lhs -= rhs;
  return lhs;
}

ModInt Pow(ModInt number, size_t power) {
  ModInt powered(1);
  for (size_t i = 0; i < power; ++i) {
    powered *= number;
  }
  return powered;
}

bool operator<(ModInt lhs, ModInt rhs) {
  for (size_t i = 0; i < ModInt::kHashRate; ++i) {
    if (lhs.numbers_[i] < rhs.numbers_[i]) {
      return true;
    }
    if (lhs.numbers_[i] > rhs.numbers_[i]) {
      return false;
    }
  }
  return false;
}

bool operator==(ModInt lhs, ModInt rhs) {
  for (size_t i = 0; i < ModInt::kHashRate; ++i) {
    if (lhs.numbers_[i] != rhs.numbers_[i]) {
      return false;
    }
  }
  return true;
}

struct Hasher {
  static constexpr uint64_t base = 317;
  static ModInt StoI(char ch) {
    return ModInt(static_cast<uint64_t>(ch - 'a' + 1));
  }
  static std::vector<ModInt> Hashes(const std::string& str) {
    std::vector<ModInt> hashes(str.length());
    hashes[0] = ModInt(StoI(str[0]));
    for (size_t i = 1; i < str.length(); ++i) {
      hashes[i] = hashes[i - 1] * base + StoI(str[i]);
    }
    return hashes;
  }
  static ModInt Hash(const std::string& str) {
    return Hashes(str)[str.length() - 1];
  }
  static ModInt Power(size_t size) { return Powers(size)[size - 1]; }

  static std::vector<ModInt> Powers(size_t size) {
    std::vector<ModInt> powers(size);
    powers[0] = ModInt(1);
    for (size_t i = 1; i < size; ++i) {
      powers[i] = powers[i - 1] * base;
    }
    return powers;
  }
};

struct Substr {
  ModInt hash;
  size_t id;
  size_t len;
};

bool operator<(const Substr& lhs, const Substr& rhs) {
  return (lhs.hash < rhs.hash) || (lhs.hash == rhs.hash && lhs.id < rhs.id) ||
         (lhs.hash == rhs.hash && lhs.id == rhs.id && lhs.len < rhs.len);
}

bool operator==(const Substr& lhs, const Substr& rhs) {
  return (lhs.hash == rhs.hash && lhs.id == rhs.id && lhs.len == rhs.len);
}

void AddPrefixes(std::vector<std::string>& strings,
                 std::vector<Substr>& prefixes) {
  for (size_t str = 0; str < strings.size(); ++str) {
    auto& string = strings[str];
    prefixes.push_back({Hasher::Hash(string), str, string.length()});
  }
}

void AddSuffixes(std::vector<std::string>& strings,
                 std::vector<Substr>& suffixes) {
  for (size_t str = 0; str < strings.size(); ++str) {
    auto string = strings[str];
    std::reverse(string.begin(), string.end());
    suffixes.push_back({Hasher::Hash(string), str, string.length()});
  }
}

bool IsPalindrome(std::string& str) {
  size_t left = 0;
  size_t right = str.length() - 1;
  while (right > 0 && left < right) {
    if (str[left] != str[right]) {
      return false;
    }
    ++left;
    --right;
  }
  return true;
}

bool PrefCompare(const Substr& lhs, const Substr& rhs) {
  return (lhs.id < rhs.id) || (lhs.id == rhs.id && lhs.len < rhs.len) ||
         (lhs.id == rhs.id && lhs.len == rhs.len && lhs.hash < rhs.hash);
}

void TryFindAppendix(size_t id, std::string append,
                     std::vector<Substr>& string_hashes,
                     std::vector<std::pair<size_t, size_t>>& answer,
                     bool reverse = false) {
  auto app_hash = Hasher::Hash(append);
  Substr target{app_hash, 0, 0};
  auto found =
      std::lower_bound(string_hashes.begin(), string_hashes.end(), target);
  while (found != string_hashes.end() && found->hash == app_hash) {
    if (id != found->id) {
      if (reverse) {
        answer.push_back({found->id + 1, id + 1});
      } else {
        answer.push_back({id + 1, found->id + 1});
      }
    }
    ++found;
  }
}

const size_t kMaxLen = 2e6 + 5;
void Solve(std::vector<std::string>& strings) {
  std::vector<Substr> string_hashes;
  AddPrefixes(strings, string_hashes);
  std::sort(string_hashes.begin(), string_hashes.end());

  std::vector<std::pair<size_t, size_t>> answer;
  for (size_t i = 0; i < strings.size(); ++i) {
    // check all suffixes
    auto string = strings[i];
    for (size_t len = 1; len <= string.length(); ++len) {
      std::string str = string;
      std::string append = str.substr(0, len);
      std::reverse(append.begin(), append.end());
      str += append;
      if (IsPalindrome(str)) {
        TryFindAppendix(i, append, string_hashes, answer);
      }
      str = string;
      append = str.substr(str.length() - len, len);
      std::reverse(append.begin(), append.end());
      str = append + str;
      if (IsPalindrome(str)) {
        TryFindAppendix(i, append, string_hashes, answer, true);
      }
    }
  }
  std::sort(answer.begin(), answer.end());
  auto last = std::unique(answer.begin(), answer.end());
  answer.erase(last, answer.end());
  std::cout << answer.size() << '\n';
  Print(answer);
}

void SetFastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

int main() {
  SetFastIO();
  size_t strings_count;
  std::cin >> strings_count;
  std::vector<std::string> strings(strings_count);
  Read(strings_count, strings);
  Solve(strings);
  return 0;
}