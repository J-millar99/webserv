#include "System.hpp"

void System::parseConfigFile(const std::string& configFile)
{
    std::ifstream file(configFile.c_str()); // 파일 스트림
    std::string line; // 읽을 줄
    std::string lineBlock; // 읽은 줄

    // 줄바꿈을 모두 제거하여 한 줄로 읽음 : 어떻게 적힐지 모르기 때문에 통일
    while (std::getline(file, line)) {
        trim(line);
        if (line.empty() || line[0] == '#')
            continue; // 주석이나 빈 줄 무시
        lineBlock += line;
    }
    splitServerBlock(lineBlock);
    file.close();
}

void System::splitServerBlock(std::string& lineBlock) {
    std::string::size_type idx = 0; // 커서 위치 인덱스
    size_t bracketCount = 0;    // 중괄호 로킹 프로토콜
    bool inServerBlock = false; //  서버 블록 판단 변수

    // lineBlock은 현재 파일 전체의 내용
    while (true) {
        char cursor = lineBlock[idx];
        if (!cursor)    break;

        if (cursor == '{')
            ++bracketCount;
        else if (cursor == '}')
            --bracketCount;

        // 서버 블록이 아닐 때 첫 '{'는 서버 블록의 시작을 의미
        if (!inServerBlock && cursor == '{') {
            inServerBlock = true;
            std::string serverDirective = lineBlock.substr(0, idx); // idx는 중괄호 기호에 위치
            trim(serverDirective); // "server {" 꼴로 입력된 부분에서 "server "만 남기고 좌우 공백 제거
            if (serverDirective != "server")    // 서버 디렉티브가 아니면 에러
                throw std::runtime_error("Missing 'server' directive");
        } else if (inServerBlock && !bracketCount) {    // 서버 블록일 때, 괄호가 모두 닫혔으면 한 개의 서버
            // 서버 한 블록을 떼내고 라인 블록은 다음 "server {"를 읽도록 위치 시킴
            std::string serverBlock = lineBlock.substr(0, idx + 1);
            lineBlock = lineBlock.substr(idx + 1, lineBlock.size() - idx);

            // 서버 생성
            Server server;
            server.parseServerBlock(serverBlock);
            servers.push_back(server);

            // 변수 초기화
            inServerBlock = false;
            idx = -1;
        }
        ++idx;
    }
    if (bracketCount)
        throw std::runtime_error("syntax error");
}
