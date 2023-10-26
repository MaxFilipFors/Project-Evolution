#include "dh.hpp"
#include <iostream>
#include <sstream>
#include <fstream>



DataHandler::DataHandler(const std::string& filename) : filename(filename) {}

bool DataHandler::readData() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open the file." << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)){
        DataEntry entry;
        std::istringstream linestream(line);
        std::string token;

        if (std::getline(linestream, token, ',')) {
            entry.company = token;
        }

        while (std::getline(linestream, token, ',')) {
            int number;
            std::istringstream(token) >> number;
            entry.stockNumber.push_back(number);
        }
    
    dataEntries.push_back(entry);
    }
    file.close();
    return true;
}

/*void DataHandler::displayData() const{
    for (const auto& entry:dataEntries) {
        std::cout << "Brand: " << entry.company << std::endl;
        std::cout << "Numbers: ";
        for (int number : entry.stockNumber) {
            std::cout << number << " ";
        }
        std::cout << "\n\n";
    }
} */

const std::vector<DataEntry>& DataHandler::getData() const {
    return dataEntries;
}