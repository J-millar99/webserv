#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "../server/Server.hpp"

#define DEFAULT_CONFIG_FILE "config/default.conf"
#define MAX_EVENTS 32

class System
{
    private:
        System();
        System(const System &ref);
        System &operator=(const System &ref);
        
        std::list<Server> servers;
        std::map<int, Server*> socket_to_server;
        int kq;
        
    public:
        System(int argc, char *argv[]);
        ~System();

        // SystemUtils
        void checkArgumentNumber(int argc) const;
        void checkConfigFileValidate(const char *configFile) const;
        void errorHandling(std::string errorString);
        void closeSocket();

        // SystemParser
        void parseConfigFile(const std::string &configFile);  
        void splitServerBlock(std::string& lineBlock);

        // System
        void runServers();
        void socketInKqueue();
        void printPort();
};

extern System* g_system;
void signalHandling(int signum);

#endif
