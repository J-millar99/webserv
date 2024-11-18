#include "System.hpp"

System::System() { /* Unused */ }
System::System(const System &ref) { /* Unused */
    (void)ref;
}
System &System::operator=(const System &ref) { /* Unused */
    (void)ref;
    return *this;
}
System::~System() { /* Unused */ }

System::System(int argc, char *argv[]) {
    if (!argv[1])
        argv[1] = (char *)DEFAULT_CONFIG_FILE;
    checkArgumentNumber(argc);
    checkConfigFileExist(argv[1]);
    parseConfigFile(argv[1]);
}

void System::parseConfigFile(const std::string &configFile)
{
    std::ifstream file(configFile.c_str());
    if (!file.is_open())
        throw std::runtime_error("Cannot open config file: " + configFile);

    std::string line;
    std::string currentBlock;
    bool inServerBlock = false;
    
    while (std::getline(file, line)) {
        trim(line);

        // 주석은 무시
        if (line.empty() || line[0] == '#')
            continue;

        // server 블록 시작 확인 (server 키워드를 찾고, 여는 중괄호가 붙어있거나, 이후 줄에서 찾아야 함)
        if (!inServerBlock && line.find("server") != std::string::npos)
        {
            inServerBlock = true; // server 블록 시작 감지
            currentBlock.clear(); // 현재 블록 초기화
            currentBlock += line; // 현재 줄을 저장
            if (countChar(line, '{') == 0) // 중괄호가 같은 줄에 없으면 계속 읽음
            {
                while (std::getline(file, line))
                {
                    trim(line);
                    if (line.empty() || line[0] == '#')
                        continue; // 주석이나 빈 줄 무시
                    currentBlock += line;
                    if (countChar(line, '{') > 0)
                        break; // 여는 중괄호를 찾으면 중단
                }
            }
        }

        // server 블록 내부 처리
        if (inServerBlock)
        {
            currentBlock += line;

            // 여는 중괄호와 닫는 중괄호의 수가 같으면 server 블록이 끝난 것
            if (countChar(currentBlock, '{') == countChar(currentBlock, '}'))
            {
                Server server;
                server.parseServerBlock(currentBlock);
                servers.push_back(server);
                inServerBlock = false; // 블록 종료
                currentBlock.clear(); // 다음 블록을 위해 초기화
            }
        }
    }
    file.close();
}