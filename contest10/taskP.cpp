#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

template <typename T>
struct ModInt {
 private:
  T number_;
  static const T kMod = 998244353;

 public:
  ModInt() : ModInt(0) {}
  ModInt(T number) : number_(number % kMod) {}

  void Mod(T& number) {
    number %= kMod;
    if (number > (kMod / 2)) {
      number -= kMod;
    }
    if (number < -(kMod / 2)) {
      number += kMod;
    }
  }

  ModInt& operator+=(ModInt other) {
    number_ += other.number_;
    Mod(number_);
    return *this;
  }

  ModInt& operator-=(ModInt other) {
    // number_ += kMod;
    number_ -= other.number_;
    Mod(number_);

    return *this;
  }

  ModInt& operator*=(ModInt other) {
    number_ *= other.number_;
    // number_ %= kMod;
    Mod(number_);
    return *this;
  }

  ModInt& operator/=(ModInt other) {
    *this *= Pow(other, kMod - 2).Int();
    return *this;
  }

  T Int() { return number_; }
  friend std::istream& operator>>(std::istream& stream, ModInt<T>& number) {
    stream >> number.number_;
    return stream;
  }

  friend std::ostream& operator<<(std::ostream& stream,
                                  const ModInt<T>& number) {
    stream << number.number_;
    return stream;
  }
};

template <typename T>
ModInt<T> operator+(ModInt<T> lhs, ModInt<T> rhs) {
  lhs += rhs;
  return lhs;
}

template <typename T>
ModInt<T> operator*(ModInt<T> lhs, ModInt<T> rhs) {
  lhs *= rhs;
  return lhs;
}

template <typename T>
ModInt<T> operator-(ModInt<T> lhs, ModInt<T> rhs) {
  lhs -= rhs;
  return lhs;
}

// bool operator<(ModInt lhs, ModInt rhs) { return lhs.number_ < rhs.number_; }

template <typename T>
ModInt<T> Pow(ModInt<T> number, size_t power) {
  ModInt<T> powered(1);
  while (power) {
    if (power & 1 == 1) {
      powered *= number;
      --power;
    }
    number *= number;
    power >>= 1;
  }
  return powered;
}
struct RootStorage {
  const int64_t kMod = 998'244'353;                // 7 * 17 * 2 ^ 23 + 1
  const int64_t prime_root = 841'431'251;          // 31 ^ (2 ^ 5)
  const int64_t kInversePrimeRoot = 667'573'957;  // prime_root ^ -1
  static const size_t kMaxLen = (1 << 18);
  const int64_t kInv2 = 499'122'177;  // 2 ^ -1
  int64_t root_powers[kMaxLen + 1];
  int64_t inverse_root_povers[kMaxLen + 1];

  // precalc powers of prime_root
  inline RootStorage() {
    root_powers[kMaxLen] = prime_root;
    inverse_root_povers[kMaxLen] = kInversePrimeRoot;
    for (size_t i = kMaxLen / 2; i >= 1; i /= 2) {
      root_powers[i] = (root_powers[i * 2] * root_powers[i * 2]) % kMod;
      inverse_root_povers[i] =
          (inverse_root_povers[i * 2] * inverse_root_povers[i * 2]) % kMod;
    }
  }
};

template <typename T>
struct FFT {
 private:
  const std::vector<T>& polynomial_;
  std::vector<T>& result_;
  RootStorage& roots_;

 public:
  FFT(const std::vector<T>& polynomial, std::vector<T>& result,
      RootStorage& roots)
      : polynomial_(polynomial), result_(result), roots_(roots) {}

  // do FFT
  void operator()(bool inverse = false) {
    transform(0, 0, 1, polynomial_.size(), inverse);
  }

  void transform(size_t left, size_t current_left, size_t step, size_t len,
                 bool inverse = false) {
    if (len == 1) {
      result_[left] = polynomial_[current_left];
      return;
    }
    size_t offset = len / 2;
    // transform even coeficients
    transform(left, current_left, step * 2, len / 2, inverse);
    // transform odd coefficients
    transform(left + offset, current_left + step, step * 2, len / 2, inverse);

    T current_root = 1;  // w ^ 0 and w ^ len
    T root =
        (inverse ? roots_.inverse_root_povers[len] : roots_.root_powers[len]);

    for (size_t i = left; i < left + offset; ++i) {
      auto even = result_[i];
      auto odd = result_[i + offset];
      result_[i] = even + current_root * odd;
      result_[i + offset] = even - current_root * odd;
      if (inverse) {  // divide coefficients by 2
        result_[i] *= roots_.kInv2;
        result_[i + offset] *= roots_.kInv2;
      }
      current_root *= root;
    }
  }
};

template <typename T>
std::vector<T> multiply(const std::vector<T>& lhs, const std::vector<T>& rhs) {
  size_t len = lhs.size();
  std::vector<T> lhs_tr(len);
  std::vector<T> rhs_tr(len);
  RootStorage storage;
  FFT transform_left(lhs, lhs_tr, storage);
  FFT transform_right(rhs, rhs_tr, storage);
  transform_left();  // transform polynomials
  transform_right();
  std::vector<T> ans_tr(len);
  std::vector<T> ans(len);
  for (size_t i = 0; i < len; ++i) {  // multiply polynomials
    ans_tr[i] = lhs_tr[i] * rhs_tr[i];
  }
  FFT transform_result(ans_tr, ans, storage);
  transform_result(true);  // inverse transformation of result
  return ans;
}

template <typename T>
void Read(std::vector<T>& dest, size_t len) {
  dest.resize(len);
  for (size_t i = 0; i < len; ++i) {
    std::cin >> dest[i];
  }
}

template <typename T>
void Print(const std::vector<T>& dest, char delim = ' ') {
  for (auto x : dest) {
    std::cout << x << delim;
  }
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

template <typename T>
void PreparePolynomials(std::vector<T>& lhs, std::vector<T>& rhs) {
  size_t len = 1 << 17;
  size_t len1;
  std::cin >> len1;
  Read(lhs, len1 + 1);
  size_t len2;
  std::cin >> len2;
  Read(rhs, len2 + 1);
  std::reverse(lhs.begin(), lhs.end());
  std::reverse(rhs.begin(), rhs.end());
  lhs.resize(len);
  rhs.resize(len);
}

int main() {
  FastIO();
  std::string str;
  std::cin >> str;
  size_t sz = 1;
  while (sz < str.length() * 2) {
    sz <<= 1;
  }
  std::vector<ModInt<int64_t>> bit_str(str.length());
  for (size_t i = 0; i < str.length(); ++i) {
    bit_str[i] = (str[i] == '1' ? 1 : 0);
  }
  // std::reverse(bit_str.begin(), bit_str.end());
  bit_str.resize(sz);
  std::vector<ModInt<int64_t>> bit_str_tr;
  auto ans = multiply(bit_str, bit_str);
  size_t total_count = 0;
  for (size_t j = 1; j < str.length() - 1; ++j) {
    if (str[j] == '1') {
      total_count += (ans[2 * j].Int() - 1) / 2;
    }
  }
  std::cout << total_count;
  return 0;
}
