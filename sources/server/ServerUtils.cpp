#include "Server.hpp"

void trim(std::string &str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) {
        str.clear();
        return ;
    }
    size_t last = str.find_last_not_of(" \t");
    str = str.substr(first, (last - first + 1));
}

void Server::parseServerBlock(const std::string &configBlock) {
    std::istringstream stream(configBlock);
    std::string line;
    bool hasListen = false;
    bool hasErrorPage = false;
    bool hasLimitClientBodySize = false;

    while (std::getline(stream, line)) {
        trim(line);
        if (line.empty()) continue;

        // 세미콜론 확인
        if (line.back() != ';')
            throw std::runtime_error("Missing semicolon at the end of the line: " + line);

        // 세미콜론 제거
        line = line.substr(0, line.size() - 1);

        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key == "listen") {
            int portValue;
            iss >> portValue;
            if (!iss || portValue <= 0)
                throw std::runtime_error("Invalid port number.");
            port = portValue;
            hasListen = true;
        }
        else if (key == "server_name") {
            std::string name;
            while (iss >> name)
                server_names.push_back(name);
        }
        else if (key == "error_page"){
            iss >> error_page;
            if (error_page.empty())
                throw std::runtime_error("Invalid error_page value.");
            hasErrorPage = true;
        }
        else if (key == "limit_client_body_size") {
            int size;
            iss >> size;
            if (!iss || size < 0)
                throw std::runtime_error("Invalid limit_client_body_size value.");
            limit_client_body_size = size;
            hasLimitClientBodySize = true;
        }
        else if (key == "location") {
            std::string locationBlock;
            std::string locLine;
            if (line.find("{") == std::string::npos)
                throw std::runtime_error("Missing opening brace for location block.");

            while (std::getline(stream, locLine)) {
                trim(locLine);
                locationBlock += locLine + "\n";
                if (locLine == "}")
                    break;
            }
            Location loc = parseLocationBlock(locationBlock);
            locations.push_back(loc);
        }
        else
            throw std::runtime_error("Unknown directive in server block: " + key);
    }
    // 필수 필드 확인
    if (!hasListen || !hasErrorPage || !hasLimitClientBodySize)
        throw std::runtime_error("Missing required fields in server block.");
}

// location 블록 파싱
Location Server::parseLocationBlock(const std::string &locationBlock) {
    Location loc;
    bool hasRoot = false;
    bool hasIndex = false;
    bool hasMethods = false;
    bool hasAutoindex = false;
    std::istringstream stream(locationBlock);
    std::string line;

    while (std::getline(stream, line)) {
        trim(line);
        if (line.empty()) continue;

        // 세미콜론 확인
        if (line.back() != ';')
            throw std::runtime_error("Missing semicolon at the end of the line in location block: " + line);
        // 세미콜론 제거
        line = line.substr(0, line.size() - 1);

        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key == "root") {
            iss >> loc.root;
            if (loc.root.empty())
                throw std::runtime_error("Invalid root value.");
            hasRoot = true;
        }
        else if (key == "index") {
            iss >> loc.index;
            if (loc.index.empty())
                throw std::runtime_error("Invalid index value.");
            hasIndex = true;
        }
        else if (key == "methods") {
            std::string method;
            loc.methods = 0; // 초기화
            while (iss >> method) {
                if (method == "GET")
                    loc.methods |= 1 << 0;
                else if (method == "POST")
                    loc.methods |= 1 << 1;
                else if (method == "DELETE")
                    loc.methods |= 1 << 2;
                else
                    throw std::runtime_error("Invalid method value.");
                hasMethods = true;
            }
        }
        else if (key == "autoindex") {
            std::string value;
            iss >> value;
            if (value == "on")
                loc.autoindex = true;
            else if (value == "off")
                loc.autoindex = false;
            else
                throw std::runtime_error("Invalid autoindex value.");
            hasAutoindex = true;
        }
        else
            throw std::runtime_error("Unknown directive in location block: " + key);
    }
    if (!hasRoot || !hasIndex || !hasMethods || !hasAutoindex)
        throw std::runtime_error("Missing required fields in server block.");
    return loc;
}