#include <iostream>
#include "dp/dataProcessor.hpp"
#include "env/environment.hpp"

template <typename T>
void printStruct(const T& myStruct) {
for (const auto& [key, value] : myStruct) {
    std::cout << key << ": " << value << std::endl;
}}

int main () {

    Universe::DataPackage data = DataProcessor::loadData("Data/datasheet.csv");
    std::vector<char> binaryData = DataProcessor::convertDataPackageToBinary(data);
    environment mainEnvironment(data, binaryData);

    char* handlerPath = "C:/Projects/AI/Self-Training/Proj/build/Debug/handler.exe";

    int exitCode = std::system(handlerPath);
    if (exitCode == 0) {
        std::cout << "Success!" << std::endl;
    } else {
        std::cout << "Whomp Whomp, Failure" << std::endl;
    }

    return 0;
}