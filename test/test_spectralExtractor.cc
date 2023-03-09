#include "helper.hpp"
#include <cassert>
#include <cstddef>
#include <vector>

void shouldCalculatePartialSum() {
  std::vector<size_t> dimensions{5, 5, 5};
  std::vector<float> inputCube = generateSequentialData(dimensions, 1);

  std::vector<float> output = partialSum(inputCube, dimensions);

  std::vector<float> expected{325, 950, 1575};
  for (int i = 0; i < expected.size(); i++) {
    assert(output[i] == expected[i]);
  }
}

int main() { shouldCalculatePartialSum(); }
