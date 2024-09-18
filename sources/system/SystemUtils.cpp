#include "System.hpp"

void System::checkArgumentNumber(int argc) const {
    if (argc > 2)
        throw std::runtime_error("Too many arguments.");
}

void System::checkConfigFileExist(const char *configFile) const {
    if (access(configFile, F_OK) == -1)
        throw std::runtime_error("Config file does not exist.");
    if (access(configFile, R_OK) == -1)
        throw std::runtime_error("Config file authorization error.");
}