#ifndef HTTPHEADER_HPP
#define HTTPHEADER_HPP

#include "../utils.hpp"

class HttpHeader
{
    private:
        HttpHeader(const HttpHeader &src);
        HttpHeader &operator=(const HttpHeader &src);

        std::map<std::string, std::string> headers;

    public:
        HttpHeader();
        ~HttpHeader();
        void addHeader(const std::string &key, const std::string &value);
        std::string getHeader(const std::string &key) const;
};

std::vector<std::string> split(const std::string &str, const std::string &delimiter);

#endif
