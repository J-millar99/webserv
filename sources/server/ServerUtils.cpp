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

bool countChar(const std::string &str, char ch) {
    int count = 0;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == ch)
            ++count;
        if (count > 1)
            return false;  // 두 개 이상 있으면 바로 false
    }
    return count == 1;  // 정확히 한 개 있는지 확인
}


void Server::parseServerBlock(const std::string &configBlock)
{
    std::istringstream stream(configBlock);
    std::string line;
    bool locationBlockStarted = false;
    std::string locationBlock;

    while (std::getline(stream, line)) {
        trim(line);

        if (line.find("listen") == 0) {
            if (line.back() != ';')
                throw std::runtime_error("Missing semicolon in 'listen' directive");
            port = std::atoi(line.substr(7, line.size() - 8).c_str()); // "listen " 이후 숫자 추출
        }
        else if (line.find("server_name") == 0) {
            if (line.back() != ';')
                throw std::runtime_error("Missing semicolon in 'server_name' directive");
            server_names.push_back(line.substr(12, line.size() - 13)); // "server_name " 이후 문자열 추출
        }
        else if (line.find("error_page") == 0) {
            if (line.back() != ';')
                throw std::runtime_error("Missing semicolon in 'error_page' directive");
            error_page = line.substr(11, line.size() - 12); // "error_page " 이후 문자열 추출
        }
        else if (line.find("limit_client_body_size") == 0) {
            if (line.back() != ';')
                throw std::runtime_error("Missing semicolon in 'limit_client_body_size' directive");
            limit_client_body_size = std::atoi(line.substr(23, line.size() - 24).c_str()); // 숫자 추출
        }
        else if (line.find("location") == 0) {
            locationBlockStarted = true;
            locationBlock = line + "\n"; // location 블록 시작
        }
        else if (locationBlockStarted) {
            locationBlock += line + "\n";
            if (line.find('}') != std::string::npos) {
                locations.push_back(parseLocationBlock(locationBlock));
                locationBlock.clear();
                locationBlockStarted = false;
            }
        }
        else
            throw std::runtime_error("Unknown directive in server block");
    }
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

void Server::printInfo() {
    std::cout << "Port: " << port << std::endl;
    std::cout << "Server names: ";
    for (std::list<std::string>::iterator it = server_names.begin(); it != server_names.end(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl;
    std::cout << "Error page: " << error_page << std::endl;
    std::cout << "Limit client body size: " << limit_client_body_size << std::endl;
    std::cout << "Locations: " << std::endl;
    for (std::list<Location>::iterator it = locations.begin(); it != locations.end(); ++it) {
        std::cout << "Root: " << it->root << std::endl;
        std::cout << "Index: " << it->index << std::endl;
        std::cout << "Methods: ";
        if (it->methods & GET)
            std::cout << "GET ";
        if (it->methods & POST)
            std::cout << "POST ";
        if (it->methods & DELETE)
            std::cout << "DELETE ";
        std::cout << std::endl;
        std::cout << "Autoindex: " << (it->autoindex ? "on" : "off") << std::endl;
    }
}