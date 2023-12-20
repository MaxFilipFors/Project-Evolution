#ifndef UNIVERSE_HPP
#define UNIVERSE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <Python.h>
#include "../params/params.hpp"
#include <unordered_map>

namespace Universe {
        
extern std::unordered_map<uint32_t, std::string> companyNumberMap;
extern std::unordered_map<std::string, uint32_t> companyMap;

struct DataEntry {
    std::string date;
    double open;
    double high;
    double low;
    double close;
    double volume;
};

struct SinglePrediction {
    uint32_t companyIndex;
    std::vector<double> predictions;
};


struct PredictionPackage {
    std::string scriptName;
    std::vector<SinglePrediction> predictionPackage;
};

struct CompanyData {
    uint32_t companyIndex;
    std::vector<DataEntry> dataPackage;
};

struct PreCompleteData {std::vector<CompanyData> CompleteData;};


bool fileExists(const std::string& path);

}
#endif // UNIVERSE_HPP```
