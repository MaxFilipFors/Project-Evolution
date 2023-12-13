#include "universe.hpp"
#include <algorithm>
#include <fstream>
#include <numeric>
#include <python.h>
#include <iostream>
#include <filesystem>
#include "../dp/dataProcessor.hpp"

namespace Universe {


using PyUniquePtr = std::unique_ptr<PyObject, PyDeleter>;


std::unordered_map<uint32_t, std::string> companyNumberMap = {
    {1, "Toyota"},
    {2, "Volvo"},
    {3, "Mercedes"},
    {4, "Porche"},
    {5, "Folkvagn"},
    {6, "BMW"},
    {7, "Subaru"},
    {8, "Tesla"},
    {9, "Saab"},
};
std::unordered_map<std::string, uint32_t> companyMap = {
    {"Toyota", 1},
    {"Volvo", 2},
    {"Mercedes", 3},
    {"Porche", 4},
    {"Folkvagn", 5},
    {"BMW", 6},
    {"Subaru", 7},
    {"Tesla", 8},
    {"Saab", 9},
};

bool fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::optional<PredictionPackage> Brain::operator()(){
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();

    std::filesystem::path path(scriptPath);
    std::string moduleName = path.stem().string();
    PyObject *pName = PyUnicode_FromString(moduleName.c_str());
    PyObject *pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != nullptr){
        PyObject *pFunc = PyObject_GetAttrString(pModule, "main");
        if (pFunc && PyCallable_Check(pFunc)){
            // Convert initialData to a Python object
            PyObject *pyDataList = PyList_New(initialData.dataPackage.size());
            for (size_t i = 0; i < initialData.dataPackage.size(); ++i){
                const auto &entry = initialData.dataPackage[i];
                PyObject *pyTuple = PyTuple_New(2);
                PyTuple_SetItem(pyTuple, 0, PyLong_FromUnsignedLong(entry.companyIndex));

                PyObject *pyStockNumbers = PyList_New(entry.stockNumbers.size());
                for (size_t j = 0; j < entry.stockNumbers.size(); ++j){
                    PyList_SetItem(pyStockNumbers, j, PyLong_FromUnsignedLong(entry.stockNumbers[j]));
                }

                PyTuple_SetItem(pyTuple, 1, pyStockNumbers);
                PyList_SetItem(pyDataList, i, pyTuple);
            }

            // Call the Python function with pyDataList
            PyObject *pArgs = PyTuple_New(1);
            PyTuple_SetItem(pArgs, 0, pyDataList); // passing pyDataList instead of binaryData

            PyObject *pResult = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);

        if (pResult != nullptr) {
            if (PyUnicode_Check(pResult)) {
                const char* resultData = PyUnicode_AsUTF8(pResult);
                if (resultData != nullptr) {
                    std::string resultStr(resultData);
                    // JSON parsing
                    try {
                        nlohmann::json resultJson = nlohmann::json::parse(resultStr);
                        PredictionPackage predictionPkg;

                            // Populate PredictionPackage
                            for (auto &[key, value] : resultJson.items()) {
                                uint32_t companyIndex = std::stoi(key);
                                double prediction = value[0]; // Assuming single prediction
                                predictionPkg.predictionPackage.push_back({companyIndex, prediction});
                            }

                            return predictionPkg;
                        }
                        catch (const std::exception &e){
                            std::cerr << "JSON parsing error: " << e.what() << std::endl;
                        }
                    }else {
                std::cerr << "Received null data from Python script." << std::endl;
            }
                }else {
            std::cerr << "Failed to convert Python bytes to string." << std::endl;
        }
            }
            else {
        std::cerr << "No valid result or non-bytes result from Python script." << std::endl;
        PyErr_Print();
    }


            // Clean-up code
            Py_XDECREF(pFunc);
            Py_DECREF(pModule);
            PyGILState_Release(gstate);
            return std::nullopt;
        }
    }
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

void EvaluationManager::printScriptResults() {
    for (const auto& pair : results) {
        const std::string& scriptName = pair.first;
        const DataPackage& dataPackage = pair.second;

        std::cout << "Script: " << scriptName << std::endl;

        for (const auto& dataEntry : dataPackage.dataPackage) {
            std::cout << "Brand Name: " << dataEntry.companyIndex << std::endl;
            std::cout << "Stock Numbers: ";

            for (int number : dataEntry.stockNumbers) {
                std::cout << number << " ";
            }
            std::cout << std::endl;
        }
    }
}

void printPredictionResults(const std::unordered_map<std::string, PredictionPackage>& scriptResults) {
    for (const auto& pair : scriptResults) {
        const std::string& scriptName = pair.first;
        const PredictionPackage& package = pair.second;

        std::cout << "Script: " << scriptName << std::endl;

        for (const auto& prediction : package.predictionPackage) {
            std::cout << "  Company Index: " << prediction.companyIndex
                      << ", Prediction: " << prediction.prediction << std::endl;
        }
    }
}

void BrainManager::loadScriptsToQueue() {
    for (uint32_t i = 0; i < maxScripts; ++i) {
        std::string scriptName = "limb" + std::to_string(i+1) + ".py";
        std::string scriptPath = limbsPath + "/" + scriptName;
        
        if (!fileExists(scriptPath)) {
            std::ifstream src("scriptParent.py", std::ios::binary);
            std::ofstream dst(scriptPath, std::ios::binary);
            dst << src.rdbuf();
        }
        if (scriptsQueue.size() >= maxScripts) break;
        scriptsQueue.push(scriptPath);
    }
}

bool Universe::BrainManager::shouldRequeueScript(int result) {
    int threshold = 50;
    return result < threshold; // Your logic here
}

void BrainManager::executeScript(const std::string& scriptPath) {
    Brain brain(scriptPath, initialData);
    auto result = brain();  // result is now std::optional<PredictionPackage>

    std::filesystem::path path(scriptPath);
    std::string scriptName = path.filename().string();

    if (result.has_value()) {
        // Directly store the result
        scriptResults[scriptName] = *result;
        std::cout << "Result stored for script: " << scriptName << std::endl;

        // Display the processed result
        /*for (const auto& prediction : result->predictionPackage) {
            std::cout << "Company Index: " << prediction.companyIndex
                      << ", Prediction: " << prediction.prediction << std::endl;
        }*/
    } else {
        std::cerr << "Script execution failed or returned no result: " << scriptPath << std::endl;
    }
}


void BrainManager::initializePython() {
    if (!Py_IsInitialized()){
        PyConfig config;
        PyStatus status;

        // Initialize the PyConfig with default values
        PyConfig_InitPythonConfig(&config);

        // Set the Python executable path
        std::wstring pythonExecutable = L"C:/Projects/Tools/python.exe";
        status = PyConfig_SetString(&config, &config.program_name, pythonExecutable.c_str());

        // Check for errors
        if (PyStatus_Exception(status)) {
            PyConfig_Clear(&config);
            Py_ExitStatusException(status);
        }

        // Initialize Python with the configuration
        status = Py_InitializeFromConfig(&config);

        // Check for errors in initialization
        if (PyStatus_Exception(status)) {
            PyConfig_Clear(&config);
            Py_ExitStatusException(status);
        }

        PyConfig_Clear(&config);
        
        // Add the 'limbs' directory to the Python search path
        PyObject* sysPath = PySys_GetObject("path"); // Borrowed reference, no need to DECREF
        PyObject* path = PyUnicode_FromString("C:/Projects/AI/Self-Training/EvolutionUtanBinary/limbs");
        PyList_Append(sysPath, path);
        Py_DECREF(path);
    }
}

void BrainManager::finalizePython() {
    if (Py_IsInitialized()){
        Py_Finalize();
    }
}

void BrainManager::runBrains() {
    initializePython();
    
    limbsPath = "C:/Projects/AI/Self-Training/EvolutionUtanBinary/limbs";
    loadScriptsToQueue();

    while (!scriptsQueue.empty() && totalScriptsRun < totalRunsLimit) {
        std::string scriptPath = scriptsQueue.front();
        scriptsQueue.pop();

        executeScript(scriptPath);
        totalScriptsRun++;

        std::cout << "Total scripts executed: " << totalScriptsRun << std::endl;


        scriptsQueue.push(scriptPath);
    }
    printPredictionResults(scriptResults);
    finalizePython();
}

}