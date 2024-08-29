#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <unistd.h>
#include "../server/Server.hpp"
#define DEFAULT_CONFIG_FILE "config/default.conf"

class System
{
    private:
        System();
        System(const System &ref);
        System &operator=(const System &ref);

    public:
        System(int argc, char *argv[]);
        ~System();

        void checkArgumentNumber(int argc) const;
        void checkConfigFileExist(const char *configFile) const;
        void checkConfigFileSyntax(const std::string &configFile) const;
        void trim(std::string &str) const;
    
        class ArgumentNumberException : public std::exception
        {
            public:
                const char *what() const throw();
        };

        class ConfigFileExistException : public std::exception
        {
            public:
                const char *what() const throw();
        };

        class ConfigFileSyntaxException : public std::exception
        {
            public:
                const char *what() const throw();
        };

        class ConfigFileAuthorizationException : public std::exception
        {
            public:
                const char *what() const throw();
        };
};

#endif
