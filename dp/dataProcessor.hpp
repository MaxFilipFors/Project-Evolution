#ifndef DP_HPP
#define DP_HPP

#include <vector>
#include <string>
#include "../uni/universe.hpp"

class DataProcessor {
public:
    static Universe::PreCompleteData loadData(const std::string& inputFilename);
    static Universe::PreCompleteData DataProcessor::loadEvalData(const std::string& inputFilename);
private:

};


#endif // DP_HPP