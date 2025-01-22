#include "Server.hpp"

void Server::parseLocationBlock(std::string &locationBlock) {
    std::string::size_type idx = 0;
    std::string::size_type start = locationBlock.find_first_of('{');
    std::string LocationDirective = locationBlock.substr(0, start);
    trim(LocationDirective);
    std::vector<std::string> locationString = splitString(LocationDirective);
    locationBlock = locationBlock.substr(start + 1, locationBlock.size() - start);
    Location loc;
    loc.methods = 0;

    if (locationString[0] != "location")
        throw std::runtime_error("Missing 'Location' directive");
    // Location 필드가 등호를 가지지 않음
    if (locationString.size() == 2) {
        if (locationString[1][0] != '/')
            throw std::runtime_error("Location url_type is not validate");
    } else if (locationString.size() == 3) {
        if (locationString[1] != "=" || locationString[2][0] != '/')
            throw std::runtime_error("Location url_type is not validate");
    } else
        throw std::runtime_error("Location url_type is not validate");


    loc.url_type = locationString[1];

    while (true) {
        char cursor = locationBlock[idx];
        if (!cursor || cursor == '}')    break;

        if (cursor == ';') {
            std::string directive = locationBlock.substr(0, idx + 1); // 세미콜론 포함
            std::vector<std::string> strs = splitString(directive);
            if (!settingLocationBlock(strs, strs.size(), loc))
                throw std::runtime_error("Unknown directive in location field");  
            locationBlock = locationBlock.substr(idx + 1, locationBlock.size() - idx);
            idx = -1;
        }
        ++idx;
    }
    locations.push_back(loc);
}

bool Server::allocateRoot(std::vector<std::string>& strs, size_t size, Location &loc) {
    if (size != 2)
        throw std::runtime_error("root field is only one parameter");
    if (strs[size - 1].back() != ';')
        throw std::runtime_error("Missing semicolon in 'root' directive");
    strs[1].erase(strs[1].size() - 1);
    loc.root = strs[1];
    return true;
}

bool Server::allocateIndex(std::vector<std::string>& strs, size_t size, Location &loc) {
    if (size != 2)
        throw std::runtime_error("index field is only one parameter");
    if (strs[size - 1].back() != ';')
        throw std::runtime_error("Missing semicolon in 'index' directive");
    strs[1].erase(strs[1].size() - 1);
    loc.index = strs[1];
    return true;
}

bool Server::allocateMethods(std::vector<std::string>& strs, size_t size, Location &loc) {
    if (size > 4)
        throw std::runtime_error("methods field has more than 4 parameters");
    if (strs[size - 1].back() != ';')
        throw std::runtime_error("Missing semicolon in 'methods' directive");
    strs[size - 1].erase(strs[size - 1].size() - 1);
    for (size_t i = 1; i < size; ++i) {
        if (strs[i] == "GET")
            loc.methods |= 1 << 0;
        else if (strs[i] == "POST")
            loc.methods |= 1 << 1;
        else if (strs[i] == "DELETE")
            loc.methods |= 1 << 2;
        else
            throw std::runtime_error("Disallow function in methods field");
    }
    return true;
}

bool Server::allocateAutoIndex(std::vector<std::string>& strs, size_t size, Location &loc) {
    if (strs[size - 1].back() != ';')
        throw std::runtime_error("Missing semicolon in 'auto_index' directive");
    if (size != 2)
        throw std::runtime_error("auto_index must have only one number");
    strs[1].erase(strs[1].size() - 1);
    if (strs[1] == "on")
        loc.auto_index = true;
    else
        loc.auto_index = false;
    return true;
}

bool Server::allocateRedirect(std::vector<std::string>& strs, size_t size, Location &loc) {
    if (size != 3)
        throw std::runtime_error("return field has more than 3 parameters");
    if (strs[size - 1].back() != ';')
        throw std::runtime_error("Missing semicolon in 'return' directive");
    strs[2].erase(strs[2].size() - 1);
    loc.redirect.url = strs[2];
    if (strs[1] == "301")
        loc.redirect.status_code = 301;
    else if (strs[1] == "302")
        loc.redirect.status_code = 302;
    else if (strs[1] == "303")
        loc.redirect.status_code = 303;
    else if (strs[1] == "307")
        loc.redirect.status_code = 307;
    else if (strs[1] == "308")
        loc.redirect.status_code = 308;
    else    // 지정되지 않은 상태 코드는 예외처리
        throw std::runtime_error("Disallow status code in return field");
    return true;
}

bool Server::settingLocationBlock(std::vector<std::string>& strs, size_t size, Location &loc) {
    // 중복은 무시
    static bool _return = false;
    static bool root = false;
    static bool index = false;
    static bool methods = false;
    static bool autoindex = false;

    if (!_return && strs[0] == "return")
        _return = allocateRedirect(strs, size, loc);
    else if (!root && strs[0] == "root")
        root = allocateRoot(strs, size, loc);
    else if (!index && strs[0] == "index")
        index = allocateIndex(strs, size, loc);
    else if (!methods && strs[0] == "methods")
        methods = allocateMethods(strs, size, loc);
    else if (!autoindex && strs[0] == "autoindex")
        autoindex = allocateAutoIndex(strs, size, loc);
    else
        return false;
    return true;
}