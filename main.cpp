#include <iostream>

const int kMaxLenght = 2e5 + 1;

int number_count;
int query_count;

int left_bound;
int right_bound;
int target;

bool answer;

int numbers[kMaxLenght];

bool BinarySearch(const int* begin, const int* end, int target) {
  int middle;
  int left_bound = *begin;
  int right_bound = *end;

  while (right_bound - left_bound > 1) {
    middle = (left_bound + right_bound) / 2;

    if (numbers[middle] <= target) {
      left_bound = middle;
    } else {
      right_bound = middle;
    }
  }

  return numbers[left_bound] == target;
}

int main() {
  std::cin >> number_count;

  for (int i = 0; i < number_count; ++i) {
    std::cin >> numbers[i];
  }

  std::cin >> query_count;

  for (int i = 0; i < query_count; ++i) {
    std::cin >> left_bound >> right_bound >> target;

    answer = BinarySearch(&left_bound, &right_bound, target);

    if (answer) {
      std::cout << "YES\n";
    } else {
      std::cout << "NO\n";
    }
  }

  return 0;
}
