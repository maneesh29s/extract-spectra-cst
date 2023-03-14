#ifndef helpers_hpp
#define helpers_hpp

#include <chrono>
#include <cstddef>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

namespace chrono = std::chrono;

class Timer {
private:
  chrono::time_point<chrono::high_resolution_clock> start;
  chrono::time_point<chrono::high_resolution_clock> end;

public:
  void start_timer() { this->start = chrono::high_resolution_clock::now(); }
  void stop_timer() { this->end = chrono::high_resolution_clock::now(); }

  std::string time_elapsed() {
    auto time_taken =
        chrono::duration_cast<chrono::milliseconds>(this->end - this->start)
            .count();

    return std::to_string(time_taken) + " ms";
  }
};

class Parameters {
public:
  std::string imageFilePath;
  std::string jsonFilePath;
  std::string outputFilePath;

  Parameters() {}

  Parameters(std::string &imageFilePath, std::string &jsonFilePath,
             std::string &outputDirPath)
      : imageFilePath(imageFilePath), jsonFilePath(jsonFilePath),
        outputFilePath(outputDirPath) {}
};

class SpectralImageSource {
public:
  std::string sourceID;
  std::vector<int64_t> slicerBegin;
  std::vector<int64_t> slicerEnd;
  std::vector<int64_t> stride;
  std::vector<int64_t> length;
  std::string stokes;

  SpectralImageSource(){};

  SpectralImageSource(std::string sid, std::vector<int64_t> sb,
                      std::vector<int64_t> se, std::vector<int64_t> st,
                      std::vector<int64_t> len, std::string sto)
      : sourceID(sid), slicerBegin(sb), slicerEnd(se), stride(st), length(len),
        stokes(sto){};

  bool operator<(const SpectralImageSource &str) const {
    for (int i = 1; i >= 0; i--) {
      if (slicerBegin[i] < str.slicerBegin[i]) {
        return true;
      }
      if (slicerBegin[i] == str.slicerBegin[i]) {
        continue;
      }
      return false;
    }
    return false;
  }
};

std::vector<float> generateSequentialData(const std::vector<int64_t> &naxis,
                                          float start) {
  int64_t totpix = 1;
  for (int64_t i = 0; i < naxis.size(); i++)
    totpix *= naxis[i];

  std::vector<float> arr(totpix);

  for (int64_t i = 0; i < arr.size(); i++) {
    // logic
    arr[i] = start + (float)i;
  }
  return arr;
}

std::vector<float> generateRandomData(const std::vector<int64_t> &naxis,
                                      float range, float offset) {
  int64_t totpix = 1;
  for (int64_t i = 0; i < naxis.size(); i++)
    totpix *= naxis[i];

  std::vector<float> arr(totpix);

  time_t seed = time(0);
  srand(seed);

  for (int64_t i = 0; i < arr.size(); i++) {
    arr[i] = offset + range * (rand() / (float)RAND_MAX);
  }

  return arr;
}

std::vector<float> partialSum(std::vector<float> cube,
                              std::vector<int64_t> dimensions) {
  std::vector<float> result{};

  int64_t z_stride = dimensions[1] * dimensions[0];
  int64_t y_stride = dimensions[0];
  for (int64_t i = 0; i < dimensions[2]; i++) {
    float sum = 0;
    for (int64_t j = 0; j < dimensions[1]; j++) {
      for (int64_t k = 0; k < dimensions[0]; k++) {
        int64_t elementIndex = (i * z_stride) + (j * y_stride) + k;
        sum += cube[elementIndex];
      }
    }
    result.push_back(sum);
  }
  return result;
}

static void readDataSize(std::ifstream &dataFile, int64_t &naxes,
                         std::vector<int64_t> &naxis, int64_t &size) {
  dataFile.read((char *)&naxes, sizeof(int64_t));

  // reading naxis one by one
  naxis.resize(naxes);
  int64_t arrSize = 1;
  for (int64_t i = 0; i < naxes; i++) {
    dataFile.read((char *)&naxis[i], sizeof(int64_t));
    arrSize *= naxis[i];
  }
  size = arrSize;
}

static void readData(std::ifstream &dataFile, std::vector<float> &data) {
  float value;
  for (int64_t i = 0; i < data.size(); i++) {
    dataFile.read((char *)&value, sizeof(float));
    data[i] = value;
  }
}

static void writeDataBinary(const std::vector<int64_t> &naxis,
                            const std::vector<float> &arr,
                            std::ofstream &writer) {
  int64_t naxes = naxis.size();
  writer.write((char *)&naxes, sizeof(int64_t));

  for (int64_t i = 0; i < naxes; i++) {
    writer.write((char *)&naxis[i], sizeof(int64_t));
  }

  for (int64_t i = 0; i < arr.size(); i++) {
    writer.write((char *)&arr[i], sizeof(float));
  }
}

#endif