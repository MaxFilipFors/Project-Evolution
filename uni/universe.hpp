#ifndef UNIVERSE_HPP
#define UNIVERSE_HPP

#include <string>
#include <vector>
#include <Python.h>

namespace Universe {

struct DataEntry {
    std::string brandName;
    std::vector<int> stockNumbers;
};

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
        Brain(const std::string& scriptPath, const std::vector<char>& binaryData) : scriptPath(scriptPath), binaryData(binaryData) {};
        int operator ()();
    private:
        std::string scriptPath;
        std::vector<char> binaryData;
        int executePythonScript();
        std::string getModuleName();
        int processPythonResult();
};

class EvaluationManager {
    public:
        void evaluate (int value);
        double average() const;
    private:
        std::vector<int> values;
};

class BrainManager {
public:
    BrainManager(const std::vector<char>& binaryData) : binaryData(binaryData){};
    void runBrains();
    double getAverageEvaluation();
private:
    std::vector<char> binaryData;
    void initializePython(); // Initializes the Python interpreter
    void finalizePython(); // Finalizes the Python interpreter
    EvaluationManager evaluationManager;
    unsigned int iterations = 5;
};

}
#endif // UNIVERSE_HPP```
