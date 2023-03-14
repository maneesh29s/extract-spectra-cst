
#ifndef spectral_extractors_hpp
#define spectral_extractors_hpp

#include "helper.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <string>
#include <vector>

std::vector<float> processSource(int64_t slicerBegin[], int64_t slicerEnd[],
                                 int64_t stride[], int64_t length[],
                                 std::vector<int64_t> naxis,
                                 std::vector<float> data) {

  std::vector<float> result(length[2]);

  int64_t z_stride = naxis[1] * naxis[0];
  int64_t y_stride = naxis[0];

  // DEBUG
  std::cout << "stride: " << z_stride << " ystride: " << y_stride << "\n";
  //////////

  for (int64_t i = slicerBegin[2]; i <= slicerEnd[2]; i++) {
    float sum = 0;
    for (int64_t j = slicerBegin[1]; j <= slicerEnd[1]; j++) {
      for (int64_t k = slicerBegin[0]; k <= slicerEnd[0]; k++) {
        int64_t elementIndex = (i * z_stride) + (j * y_stride) + k;
        sum += data[elementIndex];

        // DEBUG
        std::cout << elementIndex << " i:" << i << " j:" << j << " k:" << k;
        std::cout << "    DATA:       " << data[elementIndex] << "\n";
        /////////
      }
      std::cout << std::endl;
    }
    result[i - slicerBegin[2]] = sum;
  }
  // DEBUG
  std::cout << "partial sum" << std::endl;
  for (int i = 0; i < result.size(); i++) {
    std::cout << result[i] << ",";
  }
  std::cout << std::endl;
  ////// 

  return result;
}

void spectrumExtractionWithReadInChunks(Parameters &parameters) {
  Json::Reader jsonReader;
  Json::Value root;

  std::string imageFilePath = parameters.imageFilePath;
  std::string jsonFilePath = parameters.jsonFilePath;
  std::string outputFilePath = parameters.outputFilePath;

  std::ifstream jsonFile;
  jsonFile.open(jsonFilePath);
  if (!jsonReader.parse(jsonFile, root, false)) {
    std::cerr << jsonReader.getFormattedErrorMessages();
    exit(1);
  }

  int64_t NAXES;
  std::vector<int64_t> naxis;
  int64_t dataSize;

  std::ifstream dataFile;
  dataFile.open(imageFilePath);
  readDataSize(dataFile, NAXES, naxis, dataSize);

  std::ofstream writer;
  writer.open(outputFilePath, std::ios_base::app);

  // int64_t chunkSize = 536870912; // 2 GiB of numbers
  //  yLimit should be greater than the max y-size of the source cube
  // number of y layers to make 2 GiB, currently 5 for testing
  int64_t yLimit = 5;

  int64_t yLayersProcessed = 0;
  Json::Value::ArrayIndex sourcesProcessed = 0;
  while (yLayersProcessed < naxis[1]) {
    int64_t ySize = std::min(yLimit, naxis[1] - yLayersProcessed);
    int64_t chunkSize = naxis[0] * ySize * naxis[2];
    std::vector<float> data(chunkSize);
    readData(dataFile, data);
    yLayersProcessed += ySize;

    // DEBUG
    std::cout << "yLayersProcessed: " << yLayersProcessed << "\n";
    std::cout << "ySize: " << ySize << "\n";
    ///////

    while (sourcesProcessed < root.size() &&
           root[sourcesProcessed]["slicerEnd"][1].asInt64() <
               yLayersProcessed) {
      int64_t slicerBegin[NAXES], slicerEnd[NAXES], stride[NAXES],
          length[NAXES];

      for (Json::Value::ArrayIndex j = 0; j < NAXES; j++) {
        slicerBegin[j] = root[sourcesProcessed]["slicerBegin"][j].asInt64();
        slicerEnd[j] = root[sourcesProcessed]["slicerEnd"][j].asInt64();
        length[j] = root[sourcesProcessed]["length"][j].asInt64();
      }

      // changing Y dimension value to relative
      slicerBegin[1] -= (yLayersProcessed - ySize);
      slicerEnd[1] -= (yLayersProcessed - ySize);

      // logic to process data
      std::vector<float> result =
          processSource(slicerBegin, slicerEnd, stride, length, naxis, data);

      // DEBUG
      sourcesProcessed++;
      std::cout << "sourcesProcessed: " << sourcesProcessed << "\n";
      /////

      std::vector<int64_t> resultShape{1, 1, static_cast<int64_t>(result.size())};

      writeDataBinary(resultShape, result, writer);
    }
  }
  jsonFile.close();
  dataFile.close();
  writer.close();
}

#endif