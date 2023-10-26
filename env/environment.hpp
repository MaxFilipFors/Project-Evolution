#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <iostream>
#include "../uni/universe.hpp"

class environment{
    public:
        environment(Universe::DataPackage& dPackage, const std::vector<char>& binaryData)
        : dataPackage(dPackage), binaryData(binaryData) {
            std::cout << "success!" << std::endl;
        };

        const Universe::DataPackage& getDataPackage() const {return dataPackage;}
        const std::vector<char>& getBinaryData() const {return binaryData;}
    private:
        Universe::DataPackage dataPackage;
        std::vector<char> binaryData;
};

#endif // ENVIRONMENT_HPP