#ifndef HTTPHEADER_HPP
#define HTTPHEADER_HPP

#include "../utils.hpp"

class HttpHeader
{
    private:
        std::map<std::string, std::string> headers;

    public:
        HttpHeader();
        HttpHeader(const HttpHeader &ref);
        HttpHeader &operator=(const HttpHeader &ref);
        ~HttpHeader();
        void addHeader(const std::string &key, const std::string &value);
        std::string getHeader(const std::string &key) const;
        void printInfo() const;
};

std::vector<std::string> split(const std::string &str, const std::string &delimiter);

#endif
