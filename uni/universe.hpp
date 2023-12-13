#ifndef UNIVERSE_HPP
#define UNIVERSE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <Python.h>
#include <queue>
#include "../params/params.hpp"
#include <optional>
#include <map>
#include <unordered_map>
#include "C:/Projects/Tools/json-develop/single_include/nlohmann/json.hpp"

namespace Universe {
        
extern std::unordered_map<uint32_t, std::string> companyNumberMap;
extern std::unordered_map<std::string, uint32_t> companyMap;

struct DataEntry {
    uint32_t companyIndex;
    std::vector<uint32_t> stockNumbers;
};

struct SinglePrediction {
    uint32_t companyIndex;
    double prediction;
};

struct PredictionPackage {std::vector<SinglePrediction> predictionPackage;};
struct DataPackage {std::vector<DataEntry> dataPackage;};

struct PyDeleter {
    void operator()(PyObject* p) const {
        if (p) { 
            Py_DECREF(p); 
        }
    }
};

class Brain {
    public:
        Brain(const std::string& scriptPath, const DataPackage initialData) :
         scriptPath(scriptPath), initialData(initialData) {};
        std::optional<PredictionPackage> Brain::operator()();
    private:
        std::string scriptPath;
        DataPackage initialData;
        int executePythonScript();
        std::string getModuleName();
        int processPythonResult();
};

class EvaluationManager {
    public:
        EvaluationManager(std::unordered_map<std::string, DataPackage> scriptResults) :
         results(scriptResults) { std::cout << "EvaluationManager constructed with " << results.size() << " script results." << std::endl;};
        void evaluate (int value);
        void printScriptResults();
        double average() const;
        int EvaluationManager::size(std::vector<char>);
    private:
        std::vector<int> values;
        std::unordered_map<std::string, DataPackage> results;
};

class BrainManager {
public:
    BrainManager(DataPackage DataPackage, size_t maxScripts, size_t totalRunsLimit) :
        initialData(DataPackage), maxScripts(maxScripts), totalRunsLimit(totalRunsLimit), totalScriptsRun(0) {};

    void runBrains();
    double getAverageEvaluation();
    DataPackage results;
    std::unordered_map<std::string, PredictionPackage> scriptResults;
private:
    DataPackage initialData;
    void initializePython(); // Initializes the Python interpreter
    void finalizePython(); // Finalizes the Python interpreter
    std::queue<std::string> scriptsQueue;
    size_t maxScripts = params::maxScripts;
    size_t totalRunsLimit = params::totalRunsLimit;
    size_t totalScriptsRun;
    void loadScriptsToQueue();
    void executeScript(const std::string& scriptPath);
    bool shouldRequeueScript(const int result);
    std::string limbsPath;
};

}
#endif // UNIVERSE_HPP```
