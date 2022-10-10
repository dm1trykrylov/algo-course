#include <iostream>

const int kInf = 2e9;

bool UpdateMinNumber(int* min_number, const int* new_number) {
  bool is_updated = false;
  if (*min_number > *new_number) {
    *min_number = *new_number;
    is_updated = true;
  }
  return is_updated;
}

int main() {
  size_t number_count;
  int number;
  int min_number = kInf;
  int min_number_index = 0;

  std::cin >> number_count;

  for (size_t i = 0; i < number_count; ++i) {
    std::cin >> number;
    if (UpdateMinNumber(&min_number, &number)) {
      min_number_index = i;
    }
  }

  std::cout << min_number_index;
  return 0;
}
