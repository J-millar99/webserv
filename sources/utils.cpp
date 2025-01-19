#include "utils.hpp"

void trim(std::string &str) {
    size_t first = str.find_first_not_of(" \t");

    if (first == std::string::npos) {
        str.clear();
        return ;
    }
    size_t last = str.find_last_not_of(" \t");
    str = str.substr(first, (last - first + 1));
}

int countChar(const std::string &str, char ch) {
    int count = 0;
    std::string::size_type i;

    for (i = 0; i < str.length(); ++i)
        if (str[i] == ch)
            ++count;

    return count;
}

std::vector<std::string> splitString(std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;

    while (iss >> token) // 공백과 탭을 기준으로 자동 분할
        tokens.push_back(token);

    return tokens;
}

int stringToInt(std::string& str) {
    long result = 0;
    std::string::size_type idx = 0;

    while (idx < str.size() && std::isdigit(static_cast<unsigned char>(str[idx]))) {
        result *= 10;
        if (result > INT_MAX)
            return -1;
        result += str[idx] - '0';
        ++idx;
    }

    if (str[idx] || result > INT_MAX)
        return -1;  // int값을 넘었거나 문자열의 끝에 도달하지 못했다면
    return static_cast<int>(result);
}
