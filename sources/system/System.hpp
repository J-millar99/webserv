#ifndef SYSTEM_HPP
#define SYSTEM_HPP
#define DEFAULT_CONFIG_FILE "config/default.conf"

#include "../server/Server.hpp"

class System
{
    private:
        System();
        System(const System &ref);
        System &operator=(const System &ref);
        std::list<Server> servers;

    public:
        System(int argc, char *argv[]);
        ~System();

        void checkArgumentNumber(int argc) const;
        void checkConfigFileValidate(const char *configFile) const;
        void parseConfigFile(const std::string &configFile);  
        void splitServerBlock(std::string& lineBlock);
};

#endif
