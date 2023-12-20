#ifndef DP_HPP
#define DP_HPP

//Libraries
#include <vector>
#include <string>

//Files
#include "../uni/universe.hpp"

class DataProcessor {
public:
    static Universe::PreCompleteData loadData(const std::string& inputFilename);
    static Universe::PreCompleteData DataProcessor::loadEvalData(const std::string& inputFilename);
private:

};


#endif // DP_HPP