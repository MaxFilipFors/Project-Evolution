#ifndef DP_HPP
#define DP_HPP

#include <vector>
#include <string>
#include "../uni/universe.hpp"

class DataProcessor {
public:
    static Universe::DataPackage loadData(const std::string& inputFilename);
    static std::vector<char> serializeDataPackage(const Universe::DataPackage& dataPackage);
    static Universe::DataPackage deserializeDataPackage(const std::vector<char>& binaryData);

private:
    static std::vector<char> serializeDataEntry(const Universe::DataEntry& dataEntry);
    static Universe::DataEntry deserializeDataEntry(const std::vector<char>& binaryData, size_t& offset);
};


#endif // DP_HPP
