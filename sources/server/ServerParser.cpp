#include "Server.hpp"

// 서버 한 블록을 파싱하는 함수
void Server::parseServerBlock(std::string &serverBlock) {
    listen_flag = false;
    std::string::size_type idx = 0;
    size_t bracketCount = 0;
    bool inLocationBlock = false; // 서버 내의 로케이션 블록 판단 변수

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
    strs[size - 1].erase(strs[size - 1].size() - 1);
    if (size < 2)
        throw std::runtime_error("Listen must have only one number");
    if (size == 3) {
        if (strs[2] != "default_server")
            throw std::runtime_error("Unknown directive in listen field");
        else
            default_server = true;
    }
    port = stringToInt(strs[1]);
    if (!port)
        throw std::runtime_error("Port number must be greater than 0");
    return true;
}

bool Server::allocateServerNames(std::vector<std::string>& strs, size_t size) {
    if (strs[size - 1].back() != ';')
        throw std::runtime_error("Missing semicolon in 'server_name' directive");
    strs.back().erase(strs.back().size() - 1);
    if (size < 2)
        throw std::runtime_error("server_name field is empty");
    // 중복된 서버 이름이 있으면 추가하지 않음(무시)
    for (size_t i = 1; i < size; ++i)
        if (std::find(server_names.begin(), server_names.end(), strs[i]) == server_names.end())
            server_names.push_back(strs[i]);
    return true;
}

bool Server::allocateErrorPages(std::vector<std::string>& strs, size_t size) {
    if (strs[size - 1].back() != ';')
        throw std::runtime_error("Missing semicolon in 'error_page' directive");
    strs.back().erase(strs.back().size() - 1);

    // error_page 파라미터가 2개 이상이어야 하고, 마지막 파라미터는 에러 페이지 경로여야 함
    if (size < 2 || !checkPathFormat(strs.back()))
        throw std::runtime_error("error_page directive must have at least two parameters and last parameter must be path");
    for (size_t i = 1; i < size - 1; ++i) {
        int error_code = stringToInt(strs[i]);
        if (error_code < 0)
            throw std::runtime_error("error_page parameter must be positive number");
        error_pages[strs.back()].push_back(error_code);
    }
    return true;
}

bool Server::allocateLimitClientBodySize(std::vector<std::string>& strs, size_t size) {
    if (strs[size - 1].back() != ';')
        throw std::runtime_error("Missing semicolon in 'limit_client_body_size' directive");
    strs.back().erase(strs.back().size() - 1);
    if (size != 2)
        throw std::runtime_error("limit_client_body_size must have only one number");
    limit_client_body_size = stringToInt(strs.back());
    if (limit_client_body_size < 0)
        throw std::runtime_error("limit_client_body_size must be greater than 0");
    return true;
}

bool Server::settingServerBlock(std::vector<std::string>& strs, size_t size) {
    // listen은 반드시 한 번만 나와야 함
    if (!listen_flag && strs[0] == "listen")
        return listen_flag = allocatePort(strs, size);
    else if (listen_flag && strs[0] == "listen")
        throw std::runtime_error("listen directive must be only one");
    if (strs[0] == "server_name")
        return allocateServerNames(strs, size);
    if (strs[0] == "error_page")
        return allocateErrorPages(strs, size);
    if (strs[0] == "limit_client_body_size")
        return allocateLimitClientBodySize(strs, size);
    return false;
}
