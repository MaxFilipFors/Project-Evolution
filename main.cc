
//Libraries
#include <Python.h>
#include <iostream>
#include <fstream>
#include <string>

//Files
#include "dp/dataProcessor.hpp"
#include "uni/universe.hpp"
#include "brains/brainImpl.hpp"
#include "eval/evalHandlerImpl.hpp"

namespace params {
    size_t maxScripts = 10;
    size_t totalRunsLimit = 3;
}

int main () {

    Universe::PreCompleteData data = DataProcessor::loadData("C:/Projects/AI/Self-Training/EvolutionUtanBinary/Data/all_companies_historical_copy.csv");
        
    BrainManager brainManager(data, params::maxScripts, params::totalRunsLimit);

    brainManager.runBrains();

    Universe::PreCompleteData EvalData = DataProcessor::loadEvalData("C:/Projects/AI/Self-Training/EvolutionUtanBinary/Data/eval_data.csv"); 

    std::unordered_map<std::string, Universe::PredictionPackage> PredictionData = brainManager.getScriptResults();

    EvaluationHandler evaluationHandler(EvalData, PredictionData);
    evaluationHandler.comparePredictionsWithActualData();

    try {
        evaluationHandler.printMAPEResults();
    }   catch (const std::exception &e) {
            std::cerr << "Tried and failed" << e.what() << std::endl;
        }
    return 0;
}