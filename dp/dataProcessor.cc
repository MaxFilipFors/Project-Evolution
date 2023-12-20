#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include "../uni/universe.hpp"
#include "../env/environment.hpp"
#include "dataProcessor.hpp"
#include "C:/Projects/Tools/json-develop/single_include/nlohmann/json.hpp"


Universe::PreCompleteData DataProcessor::loadData(const std::string& inputFilename) {
    Universe::PreCompleteData preCompleteData;
    std::unordered_map<uint32_t, Universe::CompanyData> companyDataMap;
    std::ifstream inputFile(inputFilename);

    if (!inputFile) {
        std::cerr << "Error: Failed to open input file." << std::endl;
        return preCompleteData;
    }

    std::string line;
    std::getline(inputFile, line);

    while (std::getline(inputFile, line)) {
        std::istringstream ss(line);
        std::string date, open, high, low, close, volume, symbol;
        std::getline(ss, date, ',');
        std::getline(ss, open, ',');
        std::getline(ss, high, ',');
        std::getline(ss, low, ',');
        std::getline(ss, close, ',');
        std::getline(ss, volume, ',');
        std::getline(ss, symbol);

        if (Universe::companyMap.find(symbol) != Universe::companyMap.end()) {
            uint32_t companyIndex = Universe::companyMap[symbol];
            Universe::DataEntry entry{date, std::stod(open), std::stod(high), std::stod(low), std::stod(close), std::stod(volume)};
            companyDataMap[companyIndex].dataPackage.push_back(entry);
            companyDataMap[companyIndex].companyIndex = companyIndex;
        } else {
            std::cerr << "Symbol not found in map: " << symbol << std::endl;
        }
    }
    for (auto& pair : companyDataMap) {
        preCompleteData.CompleteData.push_back(pair.second);
    }
    return preCompleteData;
}

Universe::PreCompleteData DataProcessor::loadEvalData(const std::string& inputFilename) {
    Universe::PreCompleteData CompleteEvalData;
    std::unordered_map<uint32_t, Universe::CompanyData> companyDataMap;
    std::ifstream inputFile(inputFilename);

    if (!inputFile) {
        std::cerr << "Error: Failed to open input file." << std::endl;
        return CompleteEvalData;
    }

    std::string line;
    std::getline(inputFile, line);

    while (std::getline(inputFile, line)) {
        std::istringstream ss(line);
        std::string date, open, high, low, close, volume, symbol;
        std::getline(ss, date, ',');
        std::getline(ss, open, ',');
        std::getline(ss, high, ',');
        std::getline(ss, low, ',');
        std::getline(ss, close, ',');
        std::getline(ss, volume, ',');
        std::getline(ss, symbol);

        if (Universe::companyMap.find(symbol) != Universe::companyMap.end()) {
            uint32_t companyIndex = Universe::companyMap[symbol];
            Universe::DataEntry entry{date, std::stod(open), std::stod(high), std::stod(low), std::stod(close), std::stod(volume)};
            companyDataMap[companyIndex].dataPackage.push_back(entry);
            companyDataMap[companyIndex].companyIndex = companyIndex;
        } else {
            std::cerr << "Symbol not found in map: " << symbol << std::endl;
        }
    }
    for (auto& pair : companyDataMap) {
        CompleteEvalData.CompleteData.push_back(pair.second);
    }
    return CompleteEvalData;
}