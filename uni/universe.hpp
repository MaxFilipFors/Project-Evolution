#ifndef UNIVERSE_HPP
#define UNIVERSE_HPP

#include <string>
#include <vector>

namespace Universe {

struct BrandName {std::string brandName;};
/*bool operator<(const BrandName& other) const{
    return brandName < other.brandName;
} */

struct StockNumbers {std::vector<int> stockNumbers;};

struct DataEntry {std::pair<BrandName, StockNumbers> dataEntry;};

struct DataPackage {std::vector<DataEntry> dataPackage;};
}

#endif // UNIVERSE_HPP