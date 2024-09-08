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
    checkConfigFileSyntax(argv[1]);
    // Server server(argv[1]);
}