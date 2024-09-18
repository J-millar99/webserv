#include "Server.hpp"

Server::Server() { /* Unused */ }
Server::Server(const Server &ref) { /* Unused */
    (void)ref;
}
Server &Server::operator=(const Server &ref) { /* Unused */
    (void)ref;
    return *this;
}
Server::~Server() { /* Unused */ }

Server::Server(const std::string &configBlock) {
    parseServerBlock(configBlock);
}
