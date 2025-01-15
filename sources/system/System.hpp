#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "../server/Server.hpp"
#include <map>
#include <sys/time.h>

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

        // 서버 구성 필드
        int kq;
        
    public:
        System(int argc, char *argv[]);
        ~System();

        // SystemParser
        void checkArgumentNumber(int argc) const;
        void checkConfigFileValidate(const char *configFile) const;
        void parseConfigFile(const std::string &configFile);  
        void splitServerBlock(std::string& lineBlock);

        // System
        void runServers();
};

#endif
