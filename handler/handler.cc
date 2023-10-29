#include "dh/dh.hpp"
#include "eh/eh.hpp"
#include "eh/eh.cpp"
#include "process/process.cpp"
#include <iostream>
#include "C:/Projects/pybind11-install/include/pybind11/pybind11.h"

int add(int i, int j) {
    return i + j;
}

PYBIND11_MODULE(example, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring

    m.def("add", &add, "A function which adds two numbers");
}

int main() {
    
    // Start DataHandler and prepare data
    DataHandler dataHandler("data/datasheet.csv");
    if (dataHandler.readData()) {
        //dataHandler.displayData();
    }
    // Set Facit
    std::map<std::string, int> correctNumbers;
    correctNumbers["Saab"] = 10;
    correctNumbers["Tesla"] = 130;
    correctNumbers["Folkvagn"] = 150;
    correctNumbers["Subaru"] = 170;
    correctNumbers["BMW"] = 190;
    correctNumbers["Mercedes"] = 120; 
    correctNumbers["Porche"] = 130;
    correctNumbers["Toyota"] = 160;
    correctNumbers["Volvo"] = 200;
    
    for (auto entry : correctNumbers){
        std::cout << "Brand is set to : " << entry.first << " with the number: " << entry.second << std::endl; 
    }

    // Start EvolutionHandler and set correct numbers
    EvaluationHandler evaluationHandler;
    evaluationHandler.setCorrectNumbers(correctNumbers);

    auto dataEntries = dataHandler.getData();
    std::map<std::string, int> predictions = makeSimplePrediction(dataEntries);
    for (const auto& entry : predictions) {
    std::cout << "Brand: " << entry.first << ", Prediction: " << entry.second << std::endl;
    }
    if (!evaluationHandler.evaluateSurvival(predictions)) {
        return 1;
    };
    evaluationHandler.calculateMAE(predictions, correctNumbers);
    return 0;
}