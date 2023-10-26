#include "eh.hpp"
#include <iostream>

EvaluationHandler::EvaluationHandler() {};

void EvaluationHandler::setCorrectNumbers(const std::map<std::string, int>& correct) {
    correctNumbers = correct;
}

bool EvaluationHandler::evaluateSurvival(const std::map<std::string, int>& predicted) {
    for (auto& entry : predicted) {
        const std::string& brand = entry.first;
        const int& number = entry.second;
        if (correctNumbers.find(brand) == correctNumbers.end()){
            std::cout << "Mismatched brand " << brand << "\n ---Terminating---" << std::endl;
            return false;
        }
    }
    return true;
}

double EvaluationHandler::calculateMAE(const std::map<std::string, int>& predicted, const std::map<std::string, int>& correct) {
    double mae = 0.0;
    int count = 0;
    for (const auto& entry : predicted) {
        const std::string& brand = entry.first;
        if (correct.find(brand) != correct.end()) {
            int predictedNumber = entry.second;
            int correctNumber = correct.at(brand);
            mae += std::abs(predictedNumber - correctNumber);
            count++;
        }
    }
    if (count > 0) {
        mae /= count;
    }
    std::cout << "mae is: " << mae << std::endl;
    return mae;
}