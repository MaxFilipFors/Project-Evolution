#include "dh/dh.hpp"
#include "dh/dh.cpp"


#include <map>
#include <vector>

std::map<std::string, int> makeSimplePrediction (const std::vector<DataEntry>& dataEntries) {
    std::map<std::string, int> predictions;

    for (const DataEntry entry : dataEntries) {
        if (!entry.stockNumber.empty()) {
            int nextDayPrediction = entry.stockNumber.back();
            predictions[entry.company] = nextDayPrediction;
        }
    }
    return predictions;
}