#ifndef BRAINIMPL_HPP
#define BRAINIMPL_HPP

//Libraries
#include <optional>
#include <queue>
#include <fstream>


//Files
#include "../uni/universe.hpp"
#include "C:/Projects/Tools/json-develop/single_include/nlohmann/json.hpp"

class Brain {
    public:
        Brain(const std::string& scriptPath, const Universe::PreCompleteData initialData) :
         scriptPath(scriptPath), initialData(initialData) {};
        std::optional<Universe::PredictionPackage> Brain::operator()();
    private:
        std::string scriptPath;
        Universe::PreCompleteData initialData;
};

class BrainManager {
public:
    BrainManager(Universe::PreCompleteData PreCompleteData, size_t maxScripts, size_t totalRunsLimit) :
        initialData(PreCompleteData), maxScripts(maxScripts), totalRunsLimit(totalRunsLimit), totalScriptsRun(0) {};

    void runBrains();
    std::unordered_map<std::string, Universe::PredictionPackage> scriptResults;
    const std::unordered_map<std::string, Universe::PredictionPackage>& getScriptResults() const;
    static void printData(const Universe::PreCompleteData& preCompleteData);
private:
    Universe::PreCompleteData initialData;
    void initializePython();
    void finalizePython();
    std::queue<std::string> scriptsQueue;
    size_t maxScripts = params::maxScripts;
    size_t totalRunsLimit = params::totalRunsLimit;
    size_t totalScriptsRun;
    void loadScriptsToQueue();
    void executeScript(const std::string& scriptPath);
    std::string limbsPath;
};

#endif // BRAINIMPL_HPP