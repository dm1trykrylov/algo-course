// Double Hashing
#include <iostream>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

template <typename T>
void Print(std::vector<T>& data, char delim = '\n') {
  for (auto element : data) {
    std::cout << element << delim;
  }
}

struct ModInt {
 private:
  static constexpr size_t kHashRate = 2;
  uint64_t numbers_[kHashRate];
  uint64_t number1_;
  uint64_t number2_;
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

// bool operator<(ModInt lhs, ModInt rhs) { return lhs.number_ < rhs.number_; }

ModInt Pow(ModInt number, size_t power) {
  ModInt powered(1);
  for (size_t i = 0; i < power; ++i) {
    powered *= number;
  }
  return powered;
}

template <>
struct std::hash<std::pair<uint64_t, uint64_t>> {
  uint64_t operator()(
      const std::pair<uint64_t, uint64_t>& pair) const noexcept {
    uintmax_t hash = std::hash<uint64_t>{}(pair.first);
    hash <<= sizeof(uintmax_t) * 4;
    hash ^= std::hash<uint64_t>{}(pair.second);
    return std::hash<uintmax_t>{}(hash);
  }
};

struct Hasher {
  static constexpr uint64_t base = 17;
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

void Solve(std::string text, std::string pattern) {
  size_t counter = 0;
  if (pattern.length() <= text.length()) {
    std::unordered_set<std::pair<uint64_t, uint64_t>> pattern_hashes;
    size_t pattern_len = pattern.length();
    auto base_m = Hasher::Power(pattern_len + 1);
    pattern += pattern;
    auto hash_arr = Hasher::Hashes(pattern);
    ModInt cur_hash = hash_arr[pattern_len - 1];
    for (size_t i = 1; i <= pattern_len; ++i) {
      pattern_hashes.insert(cur_hash.Int());
      cur_hash = hash_arr[i + pattern_len - 1] - hash_arr[i - 1] * base_m;
    }

    auto txt_arr = Hasher::Hashes(text);
    ModInt txt_hash = txt_arr[pattern_len - 1];

    for (size_t i = 1; i < text.length() - pattern_len + 1; ++i) {
      if (pattern_hashes.find(txt_hash.Int()) != pattern_hashes.end()) {
        ++counter;
      }
      txt_hash = txt_arr[i + pattern_len - 1] - txt_arr[i - 1] * base_m;
    }
    if (pattern_hashes.find(txt_hash.Int()) != pattern_hashes.end()) {
      ++counter;
    }
  }

  std::cout << counter;
}

void SetFastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

int main() {
  SetFastIO();
  std::string text;
  std::string pattern;
  std::cin >> pattern >> text;
  Solve(text, pattern);
  return 0;
}