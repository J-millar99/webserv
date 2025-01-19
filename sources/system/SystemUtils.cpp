#include "System.hpp"

void System::checkArgumentNumber(int argc) const {
    if (argc > 2)
        throw std::runtime_error("Too many arguments.");
}

void System::checkConfigFileValidate(const char *configFile) const {
    if (access(configFile, F_OK) == -1)
        throw std::runtime_error("Config file does not exist.");
    if (access(configFile, R_OK) == -1)
        throw std::runtime_error("Config file authorization error.");
}

void System::closeSocket() {
    for (std::map<int, Server*>::iterator it = socket_to_server.begin(); it != socket_to_server.end(); ++it) {
        std::cout << "Closing socket: " << it->first << std::endl;
        close(it->first);
    }
    std::cout << std::endl;
}

void System::errorHandling(std::string errorString) {
    std::cout << errorString + ":" << strerror(errno) << std::endl;
}

void signalHandling(int signum) {
    if (signum == SIGINT) {
        std::cout << "\r" << std::flush;
        std::cout << "SIGINT signal received" << std::endl;
        g_system->closeSocket();
        exit(0);
    }
}