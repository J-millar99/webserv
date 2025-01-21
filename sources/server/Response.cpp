#include "Server.hpp"

HttpResponse Server::createHttpResponse(const HttpRequest &request) {
    HttpResponse response;
    // 상태 코드 설정
    int status_code = isValidRequest(request);
    // 버전 설정
    if (status_code != OK)
        response.createErrorPage(status_code);
    else
        response.createPage();

    return response;
}