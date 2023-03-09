#ifndef helpers_hpp
#define helpers_hpp

#include <chrono>
#include <fstream>
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
  std::string outputDirPath;

  Parameters() {}

  Parameters(std::string &imageFilePath, std::string &jsonFilePath,
             std::string &outputDirPath)
      : imageFilePath(imageFilePath), jsonFilePath(jsonFilePath),
        outputDirPath(outputDirPath) {}
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

std::vector<float> generateSequentialData(const std::vector<size_t> &naxis,
                                          float start) {
  size_t totpix = 1;
  for (size_t i = 0; i < naxis.size(); i++)
    totpix *= naxis[i];

  std::vector<float> arr(totpix);

  for (size_t i = 0; i < arr.size(); i++) {
    // logic
    arr[i] = start + (float)i;
  }
  return arr;
}

std::vector<float> generateRandomData(const std::vector<size_t> &naxis,
                                      float range, float offset) {
  size_t totpix = 1;
  for (size_t i = 0; i < naxis.size(); i++)
    totpix *= naxis[i];

  std::vector<float> arr(totpix);

  time_t seed = time(0);
  srand(seed);

  for (size_t i = 0; i < arr.size(); i++) {
    arr[i] = offset + range * (rand() / (float)RAND_MAX);
  }

  return arr;
}

std::vector<float> partialSum(std::vector<float> cube,
                              std::vector<size_t> dimensions) {
  std::vector<float> result{};

  size_t z_stride = dimensions[1] * dimensions[0];
  size_t y_stride = dimensions[0];
  for (size_t i = 0; i < dimensions[2]; i++) {
    float sum = 0;
    for (size_t j = 0; j < dimensions[1]; j++) {
      for (size_t k = 0; k < dimensions[0]; k++) {
        size_t elementIndex = (i * z_stride) + (j * y_stride) + k;
        sum += cube[elementIndex];
      }
    }
    result.push_back(sum);
  }
  return result;
}

static void readDataBinary(std::string filename, size_t &naxes,
                           std::vector<size_t> &naxis,
                           std::vector<float> &data) {
  std::ifstream dataFile;
  dataFile.open(filename);

  std::cout << "readDataBinary()" << std::endl << std::endl;

  dataFile.read((char *)&naxes, sizeof(size_t));

  // reading naxis one by one
  naxis.resize(naxes);
  size_t arrSize = 1;
  for (size_t i = 0; i < naxes; i++) {
    dataFile.read((char *)&naxis[i], sizeof(size_t));
    arrSize *= naxis[i];
  }

  // reading whole data
  data.resize(arrSize);
  float value;
  for (size_t i = 0; i < arrSize; i++) {
    dataFile.read((char *)&value, sizeof(float));
    data[i] = value;
  }

  dataFile.close();
}

static void writeDataBinary(const std::vector<size_t> &naxis,
                            const std::vector<float> &arr,
                            std::string filename) {
  std::ofstream writer;
  writer.open(filename);

  size_t naxes = naxis.size();
  writer.write((char *)&naxes, sizeof(size_t));

  for (size_t i = 0; i < naxes; i++) {
    writer.write((char *)&naxis[i], sizeof(size_t));
  }

  for (size_t i = 0; i < arr.size(); i++) {
    writer.write((char *)&arr[i], sizeof(float));
  }

  writer.close();
}

#endif