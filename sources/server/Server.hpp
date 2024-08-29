#ifndef SERVER_HPP
#define SERVER_HPP

#include <exception>
#include <iostream>
#include <vector>

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
    Method method;
    bool autoindex;
};

class Server
{
    private:
        Server();
        Server(const Server &ref);
        Server &operator=(const Server &ref);
    
    public:
        Server(std::string configFile);
        ~Server();
 
};

#endif
