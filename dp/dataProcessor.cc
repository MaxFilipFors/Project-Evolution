#include "dataProcessor.hpp"
#include "../uni/universe.hpp"
#include "../env/environment.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

Universe::DataPackage DataProcessor::loadData(const std::string& inputFilename) {
    Universe::DataPackage dataPackage;
    std::ifstream inputFile(inputFilename);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Failed to open input file." << std::endl;
    }
    else {
        std::string line;
        while (std::getline(inputFile, line)) {
            std::cout << "line is: " << line << std::endl; 
            std::istringstream ss(line);
            std::string brandName;
            std::vector<int> stockNumbers;
            Universe::DataEntry dataEntry;

            while (std::getline(ss, brandName, ',')) {
                int stockNumber;
                //fix error catching later
                if (!(ss >> stockNumber)) {
                    std::cerr << "Error: Invalid CSV format." << std::endl;
                }
                stockNumbers.push_back(stockNumber);
            }
            dataEntry.dataEntry = (std::make_pair(Universe::BrandName{brandName}, Universe::StockNumbers{stockNumbers}));
            dataPackage.dataPackage.push_back(dataEntry);
        }
        inputFile.close();
        return dataPackage;
    }
}

std::vector<char> DataProcessor::convertDataPackageToBinary(const Universe::DataPackage& dataPackage) {
    std::vector<char> binaryData;
    for (const auto& dataEntry : dataPackage.dataPackage) {
        const char* byte = reinterpret_cast<const char*>(&dataEntry);
        binaryData.insert(binaryData.end(), byte, byte + sizeof(dataEntry));
    }
    return binaryData;
}


void DataProcessor::convertToBinary() {
    // Implement binary conversion logic if needed
}

void DataProcessor::populateToEnvironment(std::map<std::string, std::vector<int>>& data) {
    // Implement logic to populate data to the environment
    // The 'data' argument may contain the processed data
}