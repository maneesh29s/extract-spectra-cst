#include "helper.hpp"
#include "spectralExtractors.hpp"
#include <string>
#include <vector>

int main() {
  std::string inputDataFile = "data/test_3d_data.dat";

  std::vector<int64_t> naxis = std::vector<int64_t>{20, 20, 20};
  std::vector<float> data = generateSequentialData(naxis, 1);
  writeDataBinary(naxis, data, inputDataFile);

  std::string inputJsonFile = "data/sample-input.json";
  std::string outputFile = "out/partial-sum.dat";

  Parameters parameters = Parameters(inputDataFile, inputJsonFile, outputFile);
  spectrumExtractionWithReadInChunks(parameters);
}