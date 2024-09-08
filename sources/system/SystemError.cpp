#include "System.hpp"

void System::checkArgumentNumber(int argc) const {
    if (argc > 2)
        throw ArgumentNumberException();
}

void System::checkConfigFileExist(const char *configFile) const {
    if (access(configFile, F_OK) == -1)
        throw ConfigFileExistException();
    if (access(configFile, R_OK) == -1)
        throw ConfigFileAuthorizationException();
}

void System::trim(std::string &str) const {
    // 좌우 공백 제거
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) {
        str.clear(); // 공백만 있는 경우 빈 문자열로
        return;
    }
    size_t last = str.find_last_not_of(" \t");
    str = str.substr(first, (last - first + 1));
}

void System::checkConfigFileSyntax(const std::string &configFile) const {
    std::ifstream config(configFile.c_str());
    std::string line;
    int openBraces = 0; // 중괄호의 균형을 체크하기 위한 변수
    bool semicolonRequired;

    while (std::getline(config, line)) {
        semicolonRequired = true; // 세미콜론이 필요한 경우로 초기화
        trim(line); if (line.empty()) continue; // 빈 줄은 무시
    
        // 중괄호 균형 체크
        for (size_t i = 0; i < line.size(); i++) {
            char c = line[i];
            if (c == '{') {
                openBraces++;
                semicolonRequired = false; // 새 블록이 열리면 세미콜론이 필요하지 않음
            } else if (c == '}') {
                if (openBraces == 0)
                    throw ConfigFileSyntaxException();
                openBraces--;
                semicolonRequired = false; // 블록이 닫힐 때도 세미콜론이 필요하지 않음
            }
        }

        if (semicolonRequired) {
            if (line.back() != ';')
                throw ConfigFileSyntaxException();
        } else // !semicolonRequired
            if (line.back() == ';')
                throw ConfigFileSyntaxException();
    }

    // 파일이 끝났을 때 중괄호가 맞지 않는 경우
    if (openBraces != 0)
        throw ConfigFileSyntaxException();
}

const char *System::ConfigFileSyntaxException::what() const throw() {
    return "Error: Config file syntax error";
}

const char *System::ArgumentNumberException::what() const throw() {
    return "Error: Too many arguments";
}

const char *System::ConfigFileExistException::what() const throw() {
    return "Error: Config file does not exist";
}

const char *System::ConfigFileAuthorizationException::what() const throw() {
    return "Error: Config file authorization error";
}