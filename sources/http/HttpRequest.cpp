#include "HttpRequest.hpp"

HttpRequest::~HttpRequest() { }
HttpRequest::HttpRequest() { }

HttpRequest::HttpRequest(const HttpRequest &ref) {
    raw_request = ref.raw_request;
    headers = ref.headers;
    method = ref.method;
    uri = ref.uri;
    version = ref.uri;
    body = ref.body;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &ref) {
    if (this != &ref) {
        raw_request = ref.raw_request;
        headers = ref.headers;
        method = ref.method;
        uri = ref.uri;
        version = ref.uri;
        body = ref.body;
    }
    return *this;
}
void HttpRequest::addRawRequest(const std::string &buffer) {
    raw_request += buffer;
}

bool HttpRequest::parseHttpRequest() {
    if (raw_request.empty())
        return false;

    // 헤더와 바디 분리
    size_t header_end = raw_request.find("\r\n\r\n");
    if (header_end == std::string::npos)
        return false;

    // 헤더 부분만 줄 단위로 분리
    std::string header_section = raw_request.substr(0, header_end);
    std::vector<std::string> lines = split(header_section, "\r\n");
    if (lines.empty())
        return false;

    // 요청 라인 파싱
    std::vector<std::string> first_line = split(lines[0], " ");
    if (first_line.size() != 3)
        return false;

    method = first_line[0];
    uri = first_line[1];
    version = first_line[2];

    // 헤더 파싱
    for (size_t i = 1; i < lines.size(); i++) {
        size_t colon_pos = lines[i].find(':');
        if (colon_pos != std::string::npos) {
            std::string key = lines[i].substr(0, colon_pos);
            std::string value = lines[i].substr(colon_pos + 1);
            
            // 앞뒤 공백 제거
            size_t value_start = value.find_first_not_of(" \t");
            if (value_start != std::string::npos)
                value = value.substr(value_start);
            
            headers.addHeader(key, value);
        }
    }

    // 바디 파싱
    body = raw_request.substr(header_end + 4); // \r\n\r\n 이후가 바디

    return true;
}

const HttpHeader &HttpRequest::getHeaders() const
{
    return headers;
}

const std::string &HttpRequest::getMethod() const
{
    return method;
}

const std::string &HttpRequest::getUri() const
{
    return uri;
}

const std::string &HttpRequest::getVersion() const
{
    return version;
}

const std::string &HttpRequest::getBody() const
{
    return body;
}

void HttpRequest::printInfo() const
{
    std::cout << "Method: " << method << std::endl;
    std::cout << "Uri: " << uri << std::endl;
    std::cout << "Version: " << version << std::endl;
    std::cout << "Headers";
    headers.printInfo();
    std::cout << "Body: " << body << std::endl;
}