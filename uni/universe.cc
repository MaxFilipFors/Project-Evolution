#include "universe.hpp"
#include <numeric>
#include <python.h>
#include <filesystem>
#include <iostream>

namespace Universe {

std::unordered_map<uint32_t, std::string> companyNumberMap = {
    {1, "AAPL"},
    {2, "MSFT"},
    {3, "GOOGL"},
    {4, "AMZN"},
    {5, "FB"},
    {6, "TSLA"},
    {7, "BRK.A"},
    {8, "V"},
    {9, "JNJ"},
    {10, "WMT"},
};
std::unordered_map<std::string, uint32_t> companyMap = {
    {"AAPL", 1},
    {"MSFT", 2},
    {"GOOGL", 3},
    {"AMZN", 4},
    {"FB", 5},
    {"TSLA", 6},
    {"BRK.A", 7},
    {"V", 8},
    {"JNJ", 9},
    {"WMT", 10},
};

bool fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

}