#ifndef DP_HPP
#define DP_HPP

#include <vector>
#include <string>
#include <map>
#include "../env/environment.hpp"
#include "../uni/universe.hpp"

namespace DataProcessor {

Universe::DataPackage loadData(const std::string& inputFilename);
bool processData();
void convertToBinary();
void populateToEnvironment(std::map<std::string, std::vector<int>>& data);
std::vector<char> convertDataPackageToBinary(const Universe::DataPackage& dataPackage);


}
#endif // DP_HPP