// 87093731
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

template <typename T>
ModInt<T> Pow(ModInt<T> number, size_t power) {
  ModInt<T> powered(1);
  while (power) {
    if ((power & 1) == 1) {
      powered *= number;
      --power;
    }
    number *= number;
    power >>= 1;
  }
  return powered;
}
struct RootStorage {
  const int64_t kMod = 998244353;               // 7 * 17 * 2 ^ 23 + 1
  const int64_t kPrimeRoot = 857779016;         // 31 ^ (2 ^ 6)
  const int64_t kInversePrimeRoot = 962405921;  // prime_root ^ -1
  static const size_t kMaxLen = (1 << 17);
  const int64_t kInv2 = 499122177;  // 2 ^ -1
  int64_t root_powers[kMaxLen + 1];
  int64_t inverse_root_povers[kMaxLen + 1];

  // precalc powers of prime_root
  inline RootStorage() {
    root_powers[kMaxLen] = kPrimeRoot;
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
    Transform(0, 0, 1, polynomial_.size(), inverse);
  }

  void Transform(size_t left, size_t current_left, size_t step, size_t len,
                 bool inverse = false) {
    if (len == 1) {
      result_[left] = polynomial_[current_left];
      return;
    }
    size_t offset = len / 2;
    // transform even coeficients
    Transform(left, current_left, step * 2, len / 2, inverse);
    // transform odd coefficients
    Transform(left + offset, current_left + step, step * 2, len / 2, inverse);

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
std::vector<T> Multiply(const std::vector<T>& lhs, const std::vector<T>& rhs) {
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
void Print(const std::vector<T>& source, char delim = ' ') {
  for (auto element : source) {
    std::cout << element << delim;
  }
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

template <typename T>
void PreparePolynomials(std::vector<T>& lhs, std::vector<T>& rhs) {
  const size_t kFixedLen = 1 << 17;
  size_t len1;
  std::cin >> len1;
  Read(lhs, len1 + 1);
  size_t len2;
  std::cin >> len2;
  Read(rhs, len2 + 1);
  std::reverse(lhs.begin(), lhs.end());
  std::reverse(rhs.begin(), rhs.end());
  lhs.resize(kFixedLen);
  rhs.resize(kFixedLen);
}

int main() {
  FastIO();
  std::vector<ModInt<int64_t>> poly_1;
  std::vector<ModInt<int64_t>> poly_2;
  PreparePolynomials(poly_1, poly_2);
  auto ans = Multiply(poly_1, poly_2);
  size_t max_deg = ans.size() - 1;
  while (ans[max_deg].Int() == 0) {
    --max_deg;
  }
  ans.resize(max_deg + 1);  // remove trailing zeroes
  std::reverse(ans.begin(), ans.end());
  std::cout << max_deg << ' ';
  Print(ans);
  return 0;
}
