#ifndef SERVER_HPP
#define SERVER_HPP

#include "../utils.hpp"

enum Method
{
    GET = 1 << 0,
    POST = 1 << 1,
    DELETE = 1 << 2
};

struct Location
{
    // 5개는 반드시 존재해야 함
    std::string urlType;
    std::string root;
    std::string index;
    int methods;
    bool autoIndex;
};

class Server
{
    private:
        Server &operator=(const Server &ref);

        // 포트, 바디사이즈, route(location)은 반드시 존재해야함.
        int port;
        std::list<std::string> server_names;
        std::list<std::string> error_pages;
        int limit_client_body_size;
        std::list<Location> locations;

    public:
        Server();
        Server(const Server &ref);
        ~Server();
        void parseServerBlock(std::string &serverBlock);
        bool settingServerBlock(std::vector<std::string>& strs, size_t size);
        void parseLocationBlock(std::string &locationBlock);
        bool settingLocationBlock(std::vector<std::string>& strs, size_t size, Location &loc);
        void checkField();
        void printInfo();
};

#endif
