#include "System.hpp"

void System::specifyDefaultServer() {
    std::vector<int> unique_tag;
    
    // 고유한 포트 번호만 저장
    for (std::list<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
        if (std::find(unique_tag.begin(), unique_tag.end(), it->getPort()) == unique_tag.end())
            unique_tag.push_back(it->getPort());

    // 각 고유 포트별로 default server 검증 및 설정
    for (size_t i = 0; i < unique_tag.size(); i++) {
        int default_count = 0;
        bool has_server = false;
        std::list<Server>::iterator first_server = servers.end();

        for (std::list<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
            if (it->getPort() == unique_tag[i]) {
                if (!has_server) {
                    first_server = it;
                    has_server = true;
                }
                if (it->getDefaultServer()) {
                    default_count++;
                }
            }
        }

        // 같은 포트에 default server가 2개 이상이면 에러
        if (default_count > 1)
            throw std::runtime_error("Multiple default servers specified for port");
        
        // default server가 없으면 첫 번째 서버를 default로 설정
        if (default_count == 0)
            first_server->setDefaultServer();
    }
}

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
    specifyDefaultServer();
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
