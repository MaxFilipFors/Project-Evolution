#include "evalHandlerImpl.hpp"
#include <fstream>
#include "C:/Projects/Tools/json-develop/single_include/nlohmann/json.hpp"
#include <sstream>
#include <cmath>


double EvaluationHandler::calculateRMSE(const std::vector<double>& predicted, const std::vector<double>& actual) {
    if (predicted.size() != actual.size() || predicted.empty()) {
        throw std::invalid_argument("Vectors must be of the same size and not empty.");
    }

    double sumSquaredErrors = 0.0;
    for (size_t i = 0; i < predicted.size(); ++i) {
        double difference = predicted[i] - actual[i];
        sumSquaredErrors += difference * difference;
    }

    double meanSquaredError = sumSquaredErrors / predicted.size();
    return std::sqrt(meanSquaredError);
}

double EvaluationHandler::calculateMAPE(const std::vector<double>& predicted, const std::vector<double>& actual) {
    if (predicted.size() != actual.size() || predicted.empty()) {
        throw std::invalid_argument("Vectors must be of the same size and not empty.");
    }

    double totalPercentageError = 0.0;
    for (size_t i = 0; i < predicted.size(); ++i) {
        if (actual[i] != 0) {
            totalPercentageError += std::abs((actual[i] - predicted[i]) / actual[i]);
        }
    }

    return (totalPercentageError / predicted.size()) * 100.0;
}


void EvaluationHandler::comparePredictionsWithActualData() {
    scriptMAPEResults.clear();
    std::vector<std::string> categories = {"Open", "High", "Low", "Close", "Volume"};

    for (const auto& [scriptName, predictionPackage] : predictionResults) {
        double totalMAPE = 0.0;
        size_t count = 0;

        for (const auto& singlePrediction : predictionPackage.predictionPackage) {
            auto companyDataIter = std::find_if(evalData.CompleteData.begin(), evalData.CompleteData.end(),
                [&singlePrediction](const Universe::CompanyData& companyData) {
                    return companyData.companyIndex == singlePrediction.companyIndex;
                });

            if (companyDataIter != evalData.CompleteData.end() && !companyDataIter->dataPackage.empty()) {
                const auto& actualData = companyDataIter->dataPackage.front();

                std::array<double, 5> actualValues = {
                    actualData.open, actualData.high, actualData.low, actualData.close, actualData.volume
                };

                for (size_t i = 0; i < actualValues.size(); ++i) {
                    try {
                        double mape = calculateMAPE({singlePrediction.predictions[i]}, {actualValues[i]});
                        std::cout << "Script: " << scriptName << ", Company Index: " << singlePrediction.companyIndex 
                                  << ", Category: " << categories[i] << ", MAPE: " << mape << std::endl;
                        totalMAPE += mape;
                    } catch (const std::invalid_argument& e) {
                        std::cerr << "Error calculating MAPE: " << e.what() << std::endl;
                        continue;
                    }
                }

                count += actualValues.size();
            }
        }

        double meanMAPE = count > 0 ? totalMAPE / count : 0.0;
        scriptMAPEResults[scriptName] = meanMAPE;
    }
}

void EvaluationHandler::printMAPEResults() const {
    for (const auto& [scriptName, mapeValue] : scriptMAPEResults) {
        std::cout << "Mean MAPE for Script " << scriptName << ": " << mapeValue << std::endl;
    }
}
