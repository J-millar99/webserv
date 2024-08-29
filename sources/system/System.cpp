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
    checkArgumentNumber(argc);
    checkConfigFileExist(argv[1]);
    checkConfigFileSyntax(argv[1]);
    // Server server(argv[1]);
}