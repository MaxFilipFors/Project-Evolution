#include <iostream>
#include <thread>

#include <Python.h>
#include "dp/dataProcessor.hpp"
#include "env/environment.hpp"
#include "uni/universe.hpp"
#include <fstream>
#include <string>


namespace params {
    size_t maxScripts = 10;
    size_t totalRunsLimit = 3;
}

int main () {

    // Load data from the datasheet
    Universe::DataPackage data = DataProcessor::loadData("C:/Projects/AI/Self-Training/Evolution/Data/datasheet.csv");
    
    // Initialize the BrainManager with the data
    Universe::BrainManager brainManager(data, params::maxScripts, params::totalRunsLimit);

    // Run the brains
    brainManager.runBrains();
    std::cout << "brainManager results are: " << sizeof(brainManager.scriptResults) << std::endl;
    //Universe::EvaluationManager evaluationManager(brainManager.scriptResults);

    //evaluationManager.printScriptResults();

    //double average = brainManager.getAverageEvaluation();
    //std::cout << "Average evaluation: " << average << std::endl;
    return 0;
}