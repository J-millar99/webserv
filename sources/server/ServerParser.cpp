#include "Server.hpp"

// 서버 한 블록을 파싱하는 함수
void Server::parseServerBlock(std::string &serverBlock) {
    std::string::size_type idx = 0;
    bool inLocationBlock = false; // 서버 내의 로케이션 블록 판단 변수
    size_t bracketCount = 0;
    // 첫 디렉티브 필드부터 읽기 위함
    std::string::size_type start = serverBlock.find_first_of('{') + 1;
    serverBlock = serverBlock.substr(start, serverBlock.size() - start);

    while (true) {
        char cursor = serverBlock[idx];
        if (!cursor)    break;

        if (cursor == '{')
            ++bracketCount;
        else if (cursor == '}')
            --bracketCount;

        // 로케이션 블럭이 아닐 때, 세미콜론은 서버의 디렉티브 필드를 의미
        if (!inLocationBlock && cursor == ';') {
            // listen, server_name, error_page, limit_client_body_size등을 읽기 위해 세미콜론에 커서를 위치
            std::string directive = serverBlock.substr(0, idx + 1);
            // 디렉티브와 필드 값을 파싱하여 서버 변수에 세팅
            std::vector<std::string> strs = splitString(directive);
            if (!settingServerBlock(strs, strs.size()))
                throw std::runtime_error("Unknown directive in server field");
            // 커서 재조정을 위해 서버 블록을 앞으로 밈
            serverBlock = serverBlock.substr(idx + 1, serverBlock.size() - idx);
            idx = -1;
        // 로케이션 블록이 아닐 때, 첫 '{'는 로케이션의 시작을 의미
        } else if (!inLocationBlock && cursor == '{') {
            inLocationBlock = true;
        // 로케이션 블록에서 중괄호가 모두 닫히면 로케이션 블록을 파싱
        } else if (inLocationBlock && !bracketCount) {
            // idx는 커서가 첫 '}'에 위치하게 된다
            std::string locationBlock = serverBlock.substr(0, idx + 1);
            // 해당 location부분을 분리하여 파싱한다
            serverBlock = serverBlock.substr(idx + 1, serverBlock.size() - idx);
            parseLocationBlock(locationBlock);
            inLocationBlock = false;
            idx = -1;
        }
        ++idx;
    }
    checkField();
}

bool Server::allocatePort(std::vector<std::string>& strs, size_t size) {
    if (strs[size - 1].back() != ';')
        throw std::runtime_error("Missing semicolon in 'listen' directive");
    if (size < 2)
        throw std::runtime_error("Listen must have only one number");
    if (size == 3)
    {
        if (strs[2] != "default_server")
            throw std::runtime_error("Unknown directive in listen field");
        else
            default_server = true;
    }
    strs[size - 1].erase(strs[size - 1].size() - 1);
    port = stringToInt(strs[1]);
    if (!port)
        throw std::runtime_error("Port number must be greater than 0");
    return true;
}

bool Server::allocateServerNames(std::vector<std::string>& strs, size_t size) {
    if (strs[size - 1].back() != ';')
        throw std::runtime_error("Missing semicolon in 'server_name' directive");
    if (size < 2)
        throw std::runtime_error("server_name field is empty");
    strs[size - 1].erase(strs[size - 1].size() - 1);
    for (size_t i = 1; i < size; ++i)
        server_names.push_back(strs[i]);
    return true;
}

bool Server::allocateErrorPages(std::vector<std::string>& strs, size_t size) {
    if (strs[size - 1].back() != ';')
        throw std::runtime_error("Missing semicolon in 'error_page' directive");
    if (size < 2)
        throw std::runtime_error("error_page field is empty");
    strs[size - 1].erase(strs[size - 1].size() - 1);
    for (size_t i = 1; i < size; ++i)
        error_pages.push_back(strs[i]);
    return true;
}

bool Server::allocateLimitClientBodySize(std::vector<std::string>& strs, size_t size) {
    if (strs[size - 1].back() != ';')
        throw std::runtime_error("Missing semicolon in 'limit_client_body_size' directive");
    if (size != 2)
        throw std::runtime_error("limit_client_body_size must have only one number");
    strs[1].erase(strs[1].size() - 1);
    limit_client_body_size = stringToInt(strs[1]);
    if (limit_client_body_size < 0)
        throw std::runtime_error("limit_client_body_size must be greater than 0");
    return true;
}


bool Server::settingServerBlock(std::vector<std::string>& strs, size_t size) {
    // 같은 필드가 여러번 나오는 경우 무시
    static bool listen = false;
    static bool server_name = false;
    static bool error_page = false;
    static bool limit_client_body_size = false;

    if (!listen && strs[0] == "listen")
        listen = allocatePort(strs, size);
    else if (strs[0] == "server_name")
        server_name = allocateServerNames(strs, size);
    else if (strs[0] == "error_page")
        error_page = allocateErrorPages(strs, size);
    else if (strs[0] == "limit_client_body_size")
        limit_client_body_size = allocateLimitClientBodySize(strs, size);
    else
        return false;
    return true;
}
