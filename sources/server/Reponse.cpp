#include "Server.hpp"

HttpResponse Server::createHttpResponse(const HttpRequest &request) {
    HttpResponse response;
    isValidRequest(request);

    // 버전 설정
    response.setVersion(request.getVersion());

    // 상태 코드 설정

    return response;
}