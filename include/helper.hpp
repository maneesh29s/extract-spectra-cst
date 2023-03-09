#ifndef helpers_hpp
#define helpers_hpp

#include <chrono>
#include <string>
#include <vector>

#include <adios2.h>
#include <casacore/casa/Arrays.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/CoordinateUtil.h>

#include "CasaImageAccess.h"
#include "FitsImageAccess.h"

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
  std::string inputImageType;
  std::string imageFilePath;
  std::string jsonFilePath;
  std::string outputImageType;
  std::string outputDirPath;

  Parameters() {}

  Parameters(std::string inputImageType, std::string &imageFilePath,
             std::string &jsonFilePath, std::string outputImageType,
             std::string &odp)
      : inputImageType(inputImageType), imageFilePath(imageFilePath),
        jsonFilePath(jsonFilePath), outputImageType(outputImageType),
        outputDirPath(odp) {
    if (outputDirPath.back() != '/') {
      outputDirPath.append("/");
    }
  }
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

#endif