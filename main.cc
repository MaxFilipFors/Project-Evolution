#include <iostream>
#include <thread>

#include <Python.h>
#include "dp/dataProcessor.hpp"
#include "env/environment.hpp"
#include "uni/universe.hpp"
#include <fstream>

int main () {
    // Load data from the datasheet
    Universe::DataPackage data = DataProcessor::loadData("C:/Projects/AI/Self-Training/Evolution/Data/datasheet.csv");
    
    // Convert the data package to binary
    std::vector<char> binaryData =  DataProcessor::serializeDataPackage(data);

    // Initialize the main environment with the data and binary data
    environment mainEnvironment(data, binaryData);

    // Initialize the BrainManager with the binary data
    Universe::BrainManager brainManager(binaryData);


    // Run the brains
    brainManager.runBrains();
    
    double average = brainManager.getAverageEvaluation();
    std::cout << "Average evaluation: " << average << std::endl;
    return 0;
}