#include "helper.hpp"
#include "spectralExtractors.hpp"
#include <string>
#include <vector>

int main() {
  std::vector<size_t> naxis = std::vector<size_t>{3, 3, 3};
  std::vector<float> data = generateSequentialData(naxis, 1);
  writeDataBinary(naxis, data, "data/test_array_data.dat");

  std::string inputDataFile = "data/test_array_data.dat";
  std::string inputJsonFile = "data/sample-input.json";
  std::string outputFile = "out/partial-sum";

  Parameters parameters = Parameters(inputDataFile, inputJsonFile, outputFile);
  spectrumExtractionWithSingleRead(parameters);
}