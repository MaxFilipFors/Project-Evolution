#include "dataProcessor.hpp"
#include "../uni/universe.hpp"
#include "../env/environment.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include "dataProcessor.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

Universe::DataPackage DataProcessor::loadData(const std::string& inputFilename) {
    Universe::DataPackage dataPackage;
    std::ifstream inputFile(inputFilename);

    if (!inputFile) {
        std::cerr << "Error: Failed to open input file." << std::endl;
        return dataPackage;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream ss(line);
        std::string brandName;
        std::getline(ss, brandName, ',');

        std::vector<int> stockNumbers;
        int stockNumber;
        while (ss >> stockNumber) {
            stockNumbers.push_back(stockNumber);
            if (ss.peek() == ',') ss.ignore();
        }

        dataPackage.dataPackage.push_back({brandName, stockNumbers});
    }
    return dataPackage;
}

std::vector<char> DataProcessor::serializeDataPackage(const Universe::DataPackage& dataPackage) {
    std::vector<char> binaryData;
    for (const auto& entry : dataPackage.dataPackage) {
        auto entryData = serializeDataEntry(entry);
        binaryData.insert(binaryData.end(), entryData.begin(), entryData.end());
    }
    return binaryData;
}

std::vector<char> DataProcessor::serializeDataEntry(const Universe::DataEntry& dataEntry) {
    std::vector<char> binaryData;

    size_t nameLength = dataEntry.brandName.size();
    binaryData.insert(binaryData.end(), reinterpret_cast<const char*>(&nameLength), reinterpret_cast<const char*>(&nameLength) + sizeof(nameLength));
    binaryData.insert(binaryData.end(), dataEntry.brandName.begin(), dataEntry.brandName.end());

    size_t numbersSize = dataEntry.stockNumbers.size();
    binaryData.insert(binaryData.end(), reinterpret_cast<const char*>(&numbersSize), reinterpret_cast<const char*>(&numbersSize) + sizeof(numbersSize));
    for (int number : dataEntry.stockNumbers) {
        binaryData.insert(binaryData.end(), reinterpret_cast<const char*>(&number), reinterpret_cast<const char*>(&number) + sizeof(number));
    }

    return binaryData;
}

Universe::DataPackage DataProcessor::deserializeDataPackage(const std::vector<char>& binaryData) {
    Universe::DataPackage dataPackage;
    size_t offset = 0;

    while (offset < binaryData.size()) {
        auto entry = deserializeDataEntry(binaryData, offset);
        dataPackage.dataPackage.push_back(entry);
    }

    return dataPackage;
}

Universe::DataEntry DataProcessor::deserializeDataEntry(const std::vector<char>& binaryData, size_t& offset) {
    Universe::DataEntry dataEntry;

    size_t nameLength;
    std::memcpy(&nameLength, &binaryData[offset], sizeof(nameLength));
    offset += sizeof(nameLength);
    dataEntry.brandName.assign(binaryData.begin() + offset, binaryData.begin() + offset + nameLength);
    offset += nameLength;

    size_t numbersSize;
    std::memcpy(&numbersSize, &binaryData[offset], sizeof(numbersSize));
    offset += sizeof(numbersSize);
    for (size_t i = 0; i < numbersSize; ++i) {
        int number;
        std::memcpy(&number, &binaryData[offset], sizeof(number));
        dataEntry.stockNumbers.push_back(number);
        offset += sizeof(number);
    }

    return dataEntry;
}
