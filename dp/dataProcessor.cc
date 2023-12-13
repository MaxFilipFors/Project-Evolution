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
        if (Universe::companyMap.find(brandName) != Universe::companyMap.end()) {
            uint32_t companyIndex = Universe::companyMap.at(brandName);
            std::cout << "\n" << brandName << " Pushed back: ";
            std::vector<uint32_t> stockNumbers;
            uint32_t stockNumber;
            while (ss >> stockNumber) {
                stockNumbers.push_back(stockNumber);
                if (ss.peek() == ',') ss.ignore();
                std::cout << " " << stockNumber;
                std::cout.flush();
            }
            dataPackage.dataPackage.push_back({companyIndex, stockNumbers});
        } else {
            std::cerr << "Brand name not found in map: " << brandName << std::endl;
        }
    }
    return dataPackage;
}
