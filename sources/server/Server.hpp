#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <list>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <string>
enum Method
{
    GET = 1 << 0,
    POST = 1 << 1,
    DELETE = 1 << 2
};

struct Location
{
    std::string root;
    std::string index;
    int methods;
    bool autoindex;
};

class Server
{
    private:
        Server &operator=(const Server &ref);

        int port;
        std::list<std::string> server_names;
        std::string error_page;
        int limit_client_body_size;
        std::list<Location> locations;

    public:
        Server();
        Server(const Server &ref);
        ~Server();
        void parseServerBlock(const std::string &configBlock);
        Location parseLocationBlock(const std::string &locationBlock);
        void printInfo();
};

void trim(std::string &str);
bool countChar(const std::string &str, char ch);
#endif
