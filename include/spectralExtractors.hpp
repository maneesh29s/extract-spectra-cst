
#ifndef spectral_extractors_hpp
#define spectral_extractors_hpp

#include "helper.hpp"
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <string>
#include <vector>

void spectrumExtractionWithSingleRead(Parameters &parameters) {
  Json::Reader jsonReader;
  Json::Value root;

  std::string imageFilePath = parameters.imageFilePath;
  std::string jsonFilePath = parameters.jsonFilePath;

  std::ifstream jsonFile;
  jsonFile.open(jsonFilePath);
  if (!jsonReader.parse(jsonFile, root, false)) {
    std::cerr << jsonReader.getFormattedErrorMessages();
    exit(1);
  }

  size_t NAXES;
  std::vector<size_t> naxis;
  std::vector<float> data;
  readDataBinary(imageFilePath, NAXES, naxis, data);

  for (Json::Value::ArrayIndex i = 0; i != root.size(); i++) {

    std::string sourceID = root[i]["sourceID"].asString();
    std::string stokes = root[i]["stokes"].asString();

    int64_t slicerBegin[NAXES];
    int64_t slicerEnd[NAXES];
    int64_t stride[NAXES];
    int64_t length[NAXES];

    for (Json::Value::ArrayIndex j = 0; j < NAXES; j++) {
      slicerBegin[j] = root[i]["slicerBegin"][j].asInt64();
      slicerEnd[j] = root[i]["slicerEnd"][j].asInt64();
      length[j] = root[i]["length"][j].asInt64();
    }

    std::vector<float> result(length[2]);

    size_t z_stride = naxis[1] * naxis[0];
    size_t y_stride = naxis[0];
    for (size_t i = slicerBegin[2]; i <= slicerEnd[2]; i++) {
      float sum = 0;
      for (size_t j = slicerBegin[1]; j <= slicerEnd[1]; j++) {
        for (size_t k = slicerBegin[0]; k <= slicerEnd[0]; k++) {
          size_t elementIndex = (i * z_stride) + (j * y_stride) + k;
          sum += data[elementIndex];
        }
      }
      result[i - slicerBegin[2]] = sum;
    }

    writeDataBinary(std::vector<size_t>{result.size()}, result, parameters.outputDirPath);
  }

  jsonFile.close();
}

#endif