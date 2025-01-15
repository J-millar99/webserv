#include "system/System.hpp"
void systemSetup(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    systemSetup(argc, argv);
    return 0;
}

void systemSetup(int argc, char *argv[]) {
    try {
        System system(argc, argv);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
}