#include "universe.hpp"
#include <algorithm>
#include <fstream>
#include <numeric>
#include <python.h>
#include <iostream>
#include <filesystem>

namespace Universe {


using PyUniquePtr = std::unique_ptr<PyObject, PyDeleter>;

int Brain::operator ()(){
    std::string moduleName = scriptPath.substr(scriptPath.find_last_of("/") + 1);
    moduleName = moduleName.substr(0, moduleName.length() - 3); // Strip '.py'

    PyUniquePtr pModule;
    PyUniquePtr pFunc;
    PyObject* pArgs;
    PyObject* pValue;
    PyObject* pResult;
    PyGILState_STATE gstate;
    {
        gstate = PyGILState_Ensure();
        // Add the parent directory of 'limbs' to the Python path
        PyUniquePtr pName(PyUnicode_DecodeFSDefault(moduleName.c_str()));
        pModule.reset(PyImport_Import(pName.get()));
        if (pModule == NULL) {
            PyErr_Print();
            return -1;
        }
        pFunc.reset(PyObject_GetAttrString(pModule.get(), "main"));
        pArgs = PyTuple_New(1);
        pValue = PyLong_FromLong(static_cast<long>(binaryData.size()));
        PyTuple_SetItem(pArgs, 0, pValue);
        pResult = PyObject_CallObject(pFunc.get(), pArgs);
        PyGILState_Release(gstate);
    }
    Py_DECREF(pArgs);
    if (pResult == NULL) {
        PyErr_Print();
        return -1;
    }
    int result = PyLong_AsLong(pResult);
    std::cout << "Python function result: " << result << std::endl; // Added print out
    Py_DECREF(pResult);
    return result;
}

void EvaluationManager::evaluate (int value) {
    values.push_back(value);
}
double EvaluationManager::average() const{
    if (values.empty()) {
        return 0.0;
    }
    double average = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    return average;
}

double BrainManager::getAverageEvaluation(){
    double average = evaluationManager.average();
    return average;
}
void BrainManager::initializePython() {
    if (!Py_IsInitialized()){
        Py_Initialize();
    }
}
void BrainManager::finalizePython() {
    if (Py_IsInitialized()){
        Py_Finalize();
    }
}

bool fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

void BrainManager::runBrains() {
    initializePython();
    std::string limbsPath = "C:/Projects/AI/Self-Training/Evolution/limbs";  // Function to determine the full path to 'limbs'
    PyRun_SimpleString(("import sys; sys.path.insert(0, '" + limbsPath + "')").c_str());
    
    std::vector<int> results;

    for (unsigned int i = 0; i < iterations; ++i) {
        std::string scriptName = "limb" + std::to_string(i+1) + ".py";
        std::string scriptPath = limbsPath + "/" + scriptName;

        if (!fileExists(scriptPath)) {
            std::ifstream src("scriptParent.py", std::ios::binary);
            std::ofstream dst(scriptPath, std::ios::binary);
            dst << src.rdbuf();
        }
        
        auto brain = std::make_unique<Brain>(scriptPath, binaryData);
        try {
            int result = (*brain)();
            results.push_back(result);
            std::cout << "Script executed successfully: " << scriptName << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Exception in running brain script: " << e.what() << std::endl;
        }
    }

    for (const auto& result : results) {
        evaluationManager.evaluate(result);
    }

    finalizePython();
}

}