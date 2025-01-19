#include "HttpHeader.hpp"


HttpHeader::HttpHeader(const HttpHeader &ref) { (void)ref; } /* Unused */
HttpHeader &HttpHeader::operator=(const HttpHeader &ref) { (void)ref; return *this; } /* Unused */
HttpHeader::~HttpHeader() { }
HttpHeader::HttpHeader() { }

void HttpHeader::addHeader(const std::string &key, const std::string &value)
{
    headers[key] = value;
}

std::string HttpHeader::getHeader(const std::string &key) const
{
    std::map<std::string, std::string>::const_iterator it = headers.find(key);
    if (it == headers.end())
        return "";
    return it->second;
}

std::vector<std::string> split(const std::string &str, const std::string &delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = 0;
    
    while ((end = str.find(delimiter, start)) != std::string::npos) {
        if (end != start) {  // 빈 문자열 제외
            tokens.push_back(str.substr(start, end - start));
        }
        start = end + delimiter.length();
    }
    
    // 마지막 토큰 처리
    if (start < str.length()) {
        tokens.push_back(str.substr(start));
    }
    
    return tokens;
}