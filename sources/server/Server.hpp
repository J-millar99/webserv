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

struct redirect
{
    std::string url;
    int status_code;
};

struct Location
{
    // 5개는 반드시 존재해야 함
    std::string url_type;
    std::string root;
    std::list<std::string> index;
    int methods;
    bool auto_index;
    struct redirect redirect;
};

class Server
{
    private:
        // 포트, 바디사이즈, route(location)은 반드시 존재해야함.
        bool listen_flag;
        bool return_flag;
        int port;
        std::list<std::string> server_names;
        std::map<std::string, std::vector<int> > error_pages;
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
        bool allocatePort(std::vector<std::string>& strs, size_t size);
        bool allocateServerNames(std::vector<std::string>& strs, size_t size);
        bool allocateErrorPages(std::vector<std::string>& strs, size_t size);
        bool allocateLimitClientBodySize(std::vector<std::string>& strs, size_t size);

        // LocationParser
        void parseLocationBlock(std::string &locationBlock);
        bool settingLocationBlock(std::vector<std::string>& strs, size_t size, Location &loc);
        bool allocateRoot(std::vector<std::string>& strs, size_t size, Location &loc);
        bool allocateIndex(std::vector<std::string>& strs, size_t size, Location &loc);
        bool allocateMethods(std::vector<std::string>& strs, size_t size, Location &loc);
        bool allocateAutoIndex(std::vector<std::string>& strs, size_t size, Location &loc);
        bool allocateRedirect(std::vector<std::string>& strs, size_t size, Location &loc);

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
