#include "Server.hpp"

void Server::parseLocationBlock(std::string &locationBlock) {
    return_flag = false;
    std::string::size_type idx = 0;
    std::string::size_type start = locationBlock.find_first_of('{');
    std::string LocationDirective = locationBlock.substr(0, start);
    trim(LocationDirective);
    std::vector<std::string> locationString = splitString(LocationDirective);
    locationBlock = locationBlock.substr(start + 1, locationBlock.size() - start);
    Location loc;

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


    loc.url_type = locationString.back();

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
    strs.back().erase(strs.back().size() - 1);
    loc.root = strs[1];
    return true;
}

bool Server::allocateIndex(std::vector<std::string>& strs, size_t size, Location &loc) {
    if (size < 2)
        throw std::runtime_error("index field have least one parameter");
    if (strs[size - 1].back() != ';')
        throw std::runtime_error("Missing semicolon in 'index' directive");
    strs.back().erase(strs.back().size() - 1);
    for (size_t i = 1; i < size; ++i)
        loc.index.push_back(strs[i]);
    return true;
}

bool Server::allocateMethods(std::vector<std::string>& strs, size_t size, Location &loc) {
    // 할당 전에 loc.methods를 0으로 초기화하는 이유는, 두 줄에 걸쳐서 methods를 설정할 경우 이전 값이 무시되기 때문
    loc.methods = 0;
    if (size > 4)
        throw std::runtime_error("methods field has more than 4 parameters");
    if (strs[size - 1].back() != ';')
        throw std::runtime_error("Missing semicolon in 'methods' directive");
    strs.back().erase(strs.back().size() - 1);
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
    strs.back().erase(strs.back().size() - 1);
    if (strs.back() == "on")
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
    if (return_flag && strs[0] == "return")
        return true;
    // return은 중복 시, 첫 번째 것만 적용 나머지 키워드는 중복 시, 마지막 것만 적용
    if (!return_flag && strs[0] == "return")
        return return_flag = allocateRedirect(strs, size, loc);
    if (strs[0] == "root")
        return allocateRoot(strs, size, loc);
    if (strs[0] == "index")
        return allocateIndex(strs, size, loc);
    if (strs[0] == "methods")
        return allocateMethods(strs, size, loc);
    if (strs[0] == "autoindex")
        return allocateAutoIndex(strs, size, loc);
    return false;
}