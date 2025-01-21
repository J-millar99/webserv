#ifndef SERVER_HPP
#define SERVER_HPP

#include "../utils.hpp"
#include "../http/HttpRequest.hpp"
#include "../http/HttpResponse.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
#define MAX_REQUEST_LINE_SIZE (4 * 1024)
#define MAX_CLIENTS 10

enum Method
{
    GET = 1 << 0,
    POST = 1 << 1,
    DELETE = 1 << 2
};

struct Location
{
    // 5개는 반드시 존재해야 함
    std::string url_type;
    std::string root;
    std::string index;
    int methods;
    bool auto_index;
};

class Server
{
    private:
        // 포트, 바디사이즈, route(location)은 반드시 존재해야함.
        int port;
        std::list<std::string> server_names;
        std::list<std::string> error_pages;
        int limit_client_body_size;
        std::list<Location> locations;
        bool default_server;

        // 서버 소켓 필드
        int server_socket;
        struct sockaddr_in server_addr;
        char buffer[BUFFER_SIZE];

    public:
        Server();
        Server &operator=(const Server &ref);
        Server(const Server &ref);
        ~Server();

        // ServerUtils
        void checkField();
        bool isServerSocket(int socket_fd) const;
        void printInfo();

        int getServerSocket() const;
        bool getDefaultServer() const;
        int getPort() const;
        void setDefaultServer();

        // ServerParser
        void parseServerBlock(std::string &serverBlock);
        bool settingServerBlock(std::vector<std::string>& strs, size_t size);
        void parseLocationBlock(std::string &locationBlock);
        bool settingLocationBlock(std::vector<std::string>& strs, size_t size, Location &loc);

        // Server
        void settingServer();
        void handleClient(int client_socket);

        // Receive
        HttpRequest recvHttpRequest(int clinet_socket);
        int isValidRequest(const HttpRequest &request);
        int isValidRequestLine(const HttpRequest &request);
        int isValidRequestBody(const HttpRequest &request);
        int isValidRequestHeader(const HttpRequest &request);

        // Response
        HttpResponse createHttpResponse(const HttpRequest &request);

};

#endif
