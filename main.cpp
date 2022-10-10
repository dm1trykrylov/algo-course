#include <cmath>
#include <iomanip>
#include <iostream>

void ProcessQueries(size_t* queries_count, const size_t* plants_count,
                    const double* plants) {
  size_t left_bound;
  size_t right_bound;
  double mean_value;
  while (*queries_count > 0) {
    std::cin >> left_bound >> right_bound;
    mean_value = plants[right_bound + 1] / plants[left_bound];
    mean_value = pow(mean_value, 1.0 / (right_bound - left_bound + 1));
    mean_value = pow(mean_value, (double)*plants_count);
    std::cout << std::fixed << std::setprecision(6) << mean_value << '\n';
    --(*queries_count);
  }
}

int main() {
  size_t plants_count;
  size_t queries_count;
  std::cin >> plants_count;

  double* plants = new double[plants_count + 1];
  plants[0] = 1.0;

  for (size_t i = 1; i <= plants_count; ++i) {
    std::cin >>
        plants[i];  // raising plants[i] to the power of 1.0/plants_count
    plants[i] =
        powl(plants[i],
             1.0 / ((double)plants_count));  // helps avoid double overflow
    plants[i] *= plants[i - 1];
  }

  std::cin >> queries_count;
  ProcessQueries(&queries_count, &plants_count, plants);

  delete[] plants;
  return 0;
}
