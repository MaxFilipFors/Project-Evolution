#ifndef EVALHANDLERIMPL_HPP
#define EVALHANDLERIMPL_HPP

//Libraries
#include <string>
#include <unordered_map>
#include <vector>

//Files
#include "../uni/universe.hpp"

class EvaluationHandler {
public:
        EvaluationHandler(const Universe::PreCompleteData& evalData, const std::unordered_map<std::string, Universe::PredictionPackage>& predictionResults) :
        evalData(evalData), predictionResults(predictionResults){};

        double EvaluationHandler::calculateRMSE(const std::vector<double>& predicted, const std::vector<double>& actual);
        double EvaluationHandler::calculateMAPE(const std::vector<double>& predicted, const std::vector<double>& actual);
        void EvaluationHandler::comparePredictionsWithActualData();
        void EvaluationHandler::printMAPEResults() const;
private:
    Universe::PreCompleteData evalData;
    std::unordered_map<std::string, Universe::PredictionPackage> predictionResults;
    std::unordered_map<std::string, double> scriptMAPEResults;

};

#endif //EVALHANDLERIMPL_HPP