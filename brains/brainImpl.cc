#include "brainImpl.hpp"

std::optional<Universe::PredictionPackage> Brain::operator()(){
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();

    std::filesystem::path path(scriptPath);
    std::string moduleName = path.stem().string();
    PyObject* pName = PyUnicode_FromString(moduleName.c_str());
    PyObject* pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != nullptr) {
        PyObject* pFunc = PyObject_GetAttrString(pModule, "main");
        if (pFunc && PyCallable_Check(pFunc)) {
            PyObject* pyDataList = PyList_New(initialData.CompleteData.size());
            for (size_t i = 0; i < initialData.CompleteData.size(); ++i) {
                const auto& companyData = initialData.CompleteData[i];
                PyObject* pyCompanyData = PyDict_New();
                PyDict_SetItemString(pyCompanyData, "companyIndex", PyLong_FromUnsignedLong(companyData.companyIndex));

                PyObject* pyEntries = PyList_New(companyData.dataPackage.size());
                for (size_t j = 0; j < companyData.dataPackage.size(); ++j) {
                    const auto& entry = companyData.dataPackage[j];
                    PyObject* pyEntry = Py_BuildValue("{s:s, s:d, s:d, s:d, s:d, s:d}", 
                                                      "date", entry.date.c_str(), 
                                                      "open", entry.open, 
                                                      "high", entry.high, 
                                                      "low", entry.low, 
                                                      "close", entry.close, 
                                                      "volume", entry.volume);
                    PyList_SetItem(pyEntries, j, pyEntry);
                }
                PyDict_SetItemString(pyCompanyData, "dataPackage", pyEntries);
                PyList_SetItem(pyDataList, i, pyCompanyData);
            }

            PyObject* pArgs = PyTuple_New(1);
            PyTuple_SetItem(pArgs, 0, pyDataList);

            PyObject* pResult = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);

        if (pResult != nullptr) {
            if (PyUnicode_Check(pResult)) {
                const char* resultData = PyUnicode_AsUTF8(pResult);
                if (resultData != nullptr) {
                    std::string resultStr(resultData);
                    try {
                        nlohmann::json resultJson = nlohmann::json::parse(resultStr);
                        Universe::PredictionPackage predictionPkg;

                        for (auto &[key, values] : resultJson.items()) {
                            uint32_t companyIndex = std::stoi(key);
                            std::vector<double> predictions;

                            for (auto& value : values) {
                                predictions.push_back(value.get<double>());
                            }

                            predictionPkg.predictionPackage.push_back({companyIndex, predictions});
                        }

                        return predictionPkg;
                    } catch (const std::exception &e) {
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


            
            Py_XDECREF(pFunc);
            Py_DECREF(pModule);
            PyGILState_Release(gstate);
            return std::nullopt;
        }
    }
}

void BrainManager::loadScriptsToQueue() {
    for (uint32_t i = 0; i < maxScripts; ++i) {
        std::string scriptName = "limb" + std::to_string(i+1) + ".py";
        std::string scriptPath = limbsPath + "/" + scriptName;
        
        if (!Universe::fileExists(scriptPath)) {
            std::ifstream src("scriptParent.py", std::ios::binary);
            std::ofstream dst(scriptPath, std::ios::binary);
            dst << src.rdbuf();
        }
        if (scriptsQueue.size() >= maxScripts) break;
        scriptsQueue.push(scriptPath);
    }
}

void BrainManager::executeScript(const std::string& scriptPath) {
    Brain brain(scriptPath, initialData);
    auto result = brain();

    std::filesystem::path path(scriptPath);
    std::string scriptName = path.filename().string();

    if (result.has_value()) {
        scriptResults[scriptName] = *result;
        std::cout << "Result stored for script: " << scriptName << std::endl;
    } else {
        std::cerr << "Script execution failed or returned no result: " << scriptPath << std::endl;
    }
}

void BrainManager::initializePython() {
    if (!Py_IsInitialized()){
        PyConfig config;
        PyStatus status;

        
        PyConfig_InitPythonConfig(&config);

        
        std::wstring pythonExecutable = L"C:/Projects/Tools/python.exe";
        status = PyConfig_SetString(&config, &config.program_name, pythonExecutable.c_str());

        
        if (PyStatus_Exception(status)) {
            PyConfig_Clear(&config);
            Py_ExitStatusException(status);
        }

        
        status = Py_InitializeFromConfig(&config);

        
        if (PyStatus_Exception(status)) {
            PyConfig_Clear(&config);
            Py_ExitStatusException(status);
        }

        PyConfig_Clear(&config);
        
        
        PyObject* sysPath = PySys_GetObject("path");
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

void printPredictionResults(const std::unordered_map<std::string, Universe::PredictionPackage>& scriptResults) {
    for (const auto& [scriptName, package] : scriptResults) {
        std::cout << "Script: " << scriptName << std::endl;

        for (const auto& prediction : package.predictionPackage) {
            std::cout << "  Company Index: " << prediction.companyIndex << std::endl;
            std::cout << "    Predictions: ";
            for (const auto& pred : prediction.predictions) {
                std::cout << pred << " ";
            }
            std::cout << std::endl;
        }
    }
}

void BrainManager::printData(const Universe::PreCompleteData& preCompleteData) {
    for (const auto& companyData : preCompleteData.CompleteData) {
        std::cout << "Company Index: " << companyData.companyIndex << std::endl;

        for (const auto& entry : companyData.dataPackage) {
            std::cout << "  Date: " << entry.date
                      << ", Open: " << entry.open
                      << ", High: " << entry.high
                      << ", Low: " << entry.low
                      << ", Close: " << entry.close
                      << ", Volume: " << entry.volume
                      << std::endl;
        }
        std::cout << std::endl;
    }
}

const std::unordered_map<std::string, Universe::PredictionPackage>& BrainManager::getScriptResults() const {
        return scriptResults;
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
    finalizePython();
}