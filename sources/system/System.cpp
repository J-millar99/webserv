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
    buildServers(argv[1]);
}

void System::buildServers(const std::string &configFile) {
    std::ifstream file(configFile.c_str());
    if (!file.is_open())
        throw std::runtime_error("Could not open config file.");

    std::string line;
    std::string currentBlock;
    bool inServerBlock = false;

    while (std::getline(file, line)) {
        trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (line == "server {") {
            if (inServerBlock)
                throw std::runtime_error("Nested server blocks are not allowed.");
            inServerBlock = true;
            currentBlock.clear();
        }
        else if (line == "}") {
            if (!inServerBlock)
                throw std::runtime_error("Unmatched closing brace in config file.");
            inServerBlock = false;
            // 서버 블록 파싱
            servers.push_back(Server(currentBlock));
        }
        else if (inServerBlock)
            currentBlock += line + "\n";
        else
            throw std::runtime_error("Unexpected content outside of server block.");
    }

    if (inServerBlock)
        throw std::runtime_error("Unclosed server block in config file.");
}