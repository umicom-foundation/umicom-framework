/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_native_connection.c
 * PURPOSE: Exercise an actual loopback request, response, timeout and listener admission on Windows and POSIX.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
typedef SOCKET TestSocket;
#define BAD_SOCKET INVALID_SOCKET
#define TEST_PROCESS _getpid
#define CLOSE_SOCKET closesocket
#define STOP_SEND SD_SEND
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
typedef int TestSocket;
#define BAD_SOCKET (-1)
#define TEST_PROCESS getpid
#define CLOSE_SOCKET close
#define STOP_SEND SHUT_WR
#endif
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static UmiStatus Echo(const UmiWebRequest *request, UmiWebResponse *response, void *context)
{
    size_t *calls = context;
    ++*calls;
    return umi_web_response_set_text(response, 200, "text/plain",
                                     request->method == UMI_HTTP_METHOD_POST ? request->body : "hello");
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1],
               *cases[] = {"get",          "head",          "post", "malformed",       "partial-close",
                           "read-timeout", "cancel-before", "stop", "loopback-policy", "invalid-timeout"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    UmiWebService *service = NULL;
    UmiWebServer *server = NULL;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_web_service_create(&service) == UMI_STATUS_OK);
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    size_t calls = 0U;
    const UmiHttpMethod methods[] = {UMI_HTTP_METHOD_GET, UMI_HTTP_METHOD_POST, UMI_HTTP_METHOD_HEAD};
    for (size_t i = 0U; i < 3U; ++i)
    {
        UmiWebRoute route;
        CHECK(umi_web_route_init(&route, methods[i], "/", Echo, &calls) == UMI_STATUS_OK);
        CHECK(umi_web_router_add(umi_web_service_router(service), &route) == UMI_STATUS_OK);
    }
    UmiWebServerConfig config = umi_web_server_config_default();
    UmiWebExchangeResult result;
    if (strcmp(mode, "loopback-policy") == 0)
    {
        strcpy(config.bind_address, "0.0.0.0");
        CHECK(umi_web_server_create(&config, service, &server) == UMI_STATUS_OK);
        CHECK(umi_web_server_start(server) == UMI_STATUS_PERMISSION_DENIED);
        goto finished;
    }
    if (strcmp(mode, "invalid-timeout") == 0)
    {
        CHECK(umi_web_server_create(&config, service, &server) == UMI_STATUS_OK);
        CHECK(UmiWebServerServeNext(server, 0U, 1U, cancel, &result) == UMI_STATUS_INVALID_ARGUMENT);
        goto finished;
    }
    UmiStatus started = UMI_STATUS_IO_ERROR;
    for (unsigned attempt = 0U; attempt < 16U; ++attempt)
    {
        config.port = (uint16_t)(20000U + (unsigned)TEST_PROCESS() % 40000U + attempt);
        CHECK(umi_web_server_create(&config, service, &server) == UMI_STATUS_OK);
        started = umi_web_server_start(server);
        if (started == UMI_STATUS_OK)
            break;
        umi_web_server_destroy(server);
        server = NULL;
    }
    CHECK(started == UMI_STATUS_OK);
    if (strcmp(mode, "stop") == 0)
    {
        CHECK(umi_web_server_stop(server) == UMI_STATUS_OK);
        CHECK(UmiWebServerServeNext(server, 10U, 10U, cancel, &result) == UMI_STATUS_INVALID_STATE);
        goto finished;
    }
    if (strcmp(mode, "cancel-before") == 0)
    {
        (void)umi_cancellation_token_request(cancel);
        CHECK(UmiWebServerServeNext(server, 100U, 100U, cancel, &result) == UMI_STATUS_CANCELLED);
        CHECK(!result.dispatched);
        goto finished;
    }
    TestSocket peer = socket(AF_INET, SOCK_STREAM, 0);
    CHECK(peer != BAD_SOCKET);
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(config.port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
#ifdef _WIN32
    DWORD timeout = 2000U;
    CHECK(setsockopt(peer, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) == 0);
    CHECK(setsockopt(peer, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout)) == 0);
#else
    struct timeval timeout = {2, 0};
    CHECK(setsockopt(peer, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == 0);
    CHECK(setsockopt(peer, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == 0);
#endif
    CHECK(connect(peer, (const struct sockaddr *)&address, sizeof(address)) == 0);
    const char *request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    if (strcmp(mode, "head") == 0)
        request = "HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    if (strcmp(mode, "post") == 0)
        request = "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5\r\n\r\nhello";
    if (strcmp(mode, "malformed") == 0)
        request = "GET / HTTP/1.1\r\n\r\n";
    if (strcmp(mode, "partial-close") == 0 || strcmp(mode, "read-timeout") == 0)
        request = "GET /";
    size_t sent = 0U, length = strlen(request);
    while (sent < length)
    {
#ifdef _WIN32
        int count = send(peer, request + sent, (int)(length - sent), 0);
#else
        ssize_t count = send(peer, request + sent, length - sent, 0);
#endif
        CHECK(count > 0);
        sent += (size_t)count;
    }
    if (strcmp(mode, "partial-close") == 0)
        CHECK(shutdown(peer, STOP_SEND) == 0);
    UmiStatus status = UmiWebServerServeNext(server, 1000U, 150U, cancel, &result);
    if (strcmp(mode, "partial-close") == 0 || strcmp(mode, "read-timeout") == 0)
    {
        CHECK(status == (strcmp(mode, "partial-close") == 0 ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_TIMEOUT));
        CHECK(!result.dispatched && calls == 0U);
        CLOSE_SOCKET(peer);
        goto finished;
    }
    CHECK(status == (strcmp(mode, "malformed") == 0 ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK));
    char response[2048];
    size_t bytes = 0U;
    for (;;)
    {
#ifdef _WIN32
        int count = recv(peer, response + bytes, (int)(sizeof(response) - 1U - bytes), 0);
#else
        ssize_t count = recv(peer, response + bytes, sizeof(response) - 1U - bytes, 0);
#endif
        CHECK(count >= 0);
        if (count == 0)
            break;
        bytes += (size_t)count;
        CHECK(bytes < sizeof(response) - 1U);
    }
    CLOSE_SOCKET(peer);
    response[bytes] = '\0';
    CHECK(result.response_complete && result.bytes_sent == bytes);
    CHECK(strstr(response, "Connection: close\r\n") != NULL);
    if (strcmp(mode, "malformed") == 0)
        CHECK(calls == 0U && result.response_status == 400);
    else
    {
        CHECK(calls == 1U && umi_web_server_state(server)->requests == 1U);
        char *body = strstr(response, "\r\n\r\n");
        CHECK(body != NULL);
        body += 4U;
        CHECK(strcmp(body, strcmp(mode, "head") == 0 ? "" : "hello") == 0);
    }
    CHECK(umi_web_server_state(server)->phase == UMI_WEB_SERVER_READY);
finished:
    umi_web_server_destroy(server);
    umi_web_service_destroy(service);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
