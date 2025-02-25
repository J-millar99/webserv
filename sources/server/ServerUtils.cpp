#include "Server.hpp"

void Server::printInfo() {
    int cnt = 0;
    std::cout << "Server is running ont port " << port << std::endl << std::endl;

    std::cout << "server_names: ";
    for (std::list<std::string>::iterator it = server_names.begin(); it != server_names.end(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl << std::endl;
    std::cout << "limit_client_body_size: " << limit_client_body_size << std::endl << std::endl;

    std::cout << "=====error_pages=====" << std::endl;
    for (std::map<std::string, std::vector<int> >::iterator it = error_pages.begin(); it != error_pages.end(); it++) {
        std::cout << it->first << " ";
        for (std::vector<int>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            std::cout << *it2 << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
    

    std::cout << "=====locations=====" << std::endl;
    for (std::list<Location>::iterator it = locations.begin(); it != locations.end(); it++) {
        std::cout << "location " << ++cnt << std::endl;
        std::cout << "url_type: ";
        if (it->url_type == PREFIX)
            std::cout << "prefix" << std::endl;
        else if (it->url_type == EXACT)
            std::cout << "exact" << std::endl;
        std::cout << "url: " << it->url << std::endl;
        if (it->redirect.url != "") {
            std::cout << "redirect url: " << it->redirect.url << std::endl;
            std::cout << "redirect status code: " << it->redirect.status_code << std::endl << std::endl;
        } else {
            std::cout << "root: " << it->root << std::endl;
            std::cout << "index: ";
            for (std::list<std::string>::iterator it2 = it->index.begin(); it2 != it->index.end(); it2++)
                std::cout << *it2 << " ";
            std::cout << std::endl;
            std::cout << "methods: ";
            if (it->methods & GET)
                std::cout << "GET";
            if (it->methods & POST)
                std::cout << "POST";
            if (it->methods & DELETE)
                std::cout << "DELETE";
            std::cout << std::endl;
            std::cout << "autoindex: " << std::boolalpha << it->auto_index << std::endl;
        }
        std::cout << std::endl;
    }

}

void Server::checkField() {
    if (port < 1)
        throw std::runtime_error("port is not allocated");
    else if (limit_client_body_size == -1)
        throw std::runtime_error("limit_client_body_size is not allocated");
    for (std::list<Location>::iterator it = locations.begin(); it != locations.end(); it++) {
        if (!return_flag && (it->url == "" || it->root == "" || it->index.size() == 0 || it->methods == 0))
            throw std::runtime_error("all location directives must have be defined");
    }
}

int Server::getPort() const {
    return port;
}

int Server::getServerSocket() const {
    return this->server_socket;
}

bool Server::isServerSocket(int socket_fd) const {
    return socket_fd == server_socket; 
}

bool Server::getDefaultServer() const{
    return default_server;
}

void Server::setDefaultServer() {
    default_server = true;
}

bool Server::isAllowedUri(const std::string &request_uri) {
    if (request_uri.find(".css") != std::string::npos ||
        request_uri.find(".svg") != std::string::npos ||
        request_uri.find(".jpg") != std::string::npos ||
        request_uri.find(".png") != std::string::npos)
        return true;
    return false;
}