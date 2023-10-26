#ifndef DH_HPP
#define DH_HPP

#include <vector>
#include <string>

struct DataEntry {
    std::string company;
    std::vector<int> stockNumber;
};

class DataHandler {
    public:
        DataHandler(const std::string& filename);
        bool readData();
        //void displayData() const;
        const std::vector<DataEntry>& getData() const;
    private:
        std::string filename;
        std::vector<DataEntry> dataEntries;
};


#endif // DH_HPP