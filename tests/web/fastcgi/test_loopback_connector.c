/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/fastcgi/test_loopback_connector.c
 *
 * PURPOSE:
 *   Verify fresh loopback transports, bounded waits and clean lease retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/fastcgi_loopback.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET TestSocket;
typedef int AddressSize;
#define INVALID_TEST_SOCKET INVALID_SOCKET
static void CloseTestSocket(TestSocket socket) { (void)closesocket(socket); }
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int TestSocket;
typedef socklen_t AddressSize;
#define INVALID_TEST_SOCKET (-1)
static void CloseTestSocket(TestSocket socket) { (void)close(socket); }
#endif

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%d: %s\n", __LINE__, #condition); failed = 1; goto finish; } \
} while (0)

static TestSocket Bind(uint16_t *port, bool reachable)
{
    TestSocket server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == INVALID_TEST_SOCKET) return server;
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    AddressSize size = (AddressSize)sizeof address;
    if (bind(server, (const struct sockaddr *)&address, size) != 0 ||
        getsockname(server, (struct sockaddr *)&address, &size) != 0 ||
        (reachable && listen(server, 4) != 0)) {
        CloseTestSocket(server);
        return INVALID_TEST_SOCKET;
    }
    *port = ntohs(address.sin_port);
    return server;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    int failed = 0;
#ifdef _WIN32
    bool winsockReady = false;
#endif
    TestSocket listener = INVALID_TEST_SOCKET, peer = INVALID_TEST_SOCKET;
    UmiCancellationToken *cancel = NULL;
    UmiWebFastCgiLoopbackEndpoint endpoint = {.port = 1U, .timeoutMs = 1000U};
    UmiWebFastCgiConnector connector = {0};
    UmiWebConnectionIo first = {0}, second = {0};

    if (!strcmp(mode, "config")) {
        const UmiWebFastCgiConnector before = {.context = &endpoint};
        connector = before;
        CHECK(UmiWebFastCgiLoopbackConnectorInit(NULL, &connector) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiWebFastCgiLoopbackConnectorInit(&endpoint, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        endpoint.port = 0U;
        CHECK(UmiWebFastCgiLoopbackConnectorInit(&endpoint, &connector) == UMI_STATUS_INVALID_ARGUMENT);
        endpoint.port = 1U;
        endpoint.timeoutMs = 0U;
        CHECK(UmiWebFastCgiLoopbackConnectorInit(&endpoint, &connector) == UMI_STATUS_INVALID_ARGUMENT);
        endpoint.timeoutMs = UMI_WEB_FASTCGI_LOOPBACK_MAX_WAIT_MS + 1U;
        CHECK(UmiWebFastCgiLoopbackConnectorInit(&endpoint, &connector) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(connector.context == before.context && connector.open == NULL && connector.close == NULL);
        goto finish;
    }

    if (!strcmp(mode, "cancel-before")) {
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        CHECK(UmiWebFastCgiLoopbackConnectorInit(&endpoint, &connector) == UMI_STATUS_OK);
        first.context = &endpoint;
        CHECK(connector.open(connector.context, cancel, &first) == UMI_STATUS_CANCELLED);
        CHECK(first.context == &endpoint && first.read == NULL && first.write == NULL);
        first.context = NULL;
        goto finish;
    }

#ifdef _WIN32
    WSADATA data;
    CHECK(WSAStartup(MAKEWORD(2, 2), &data) == 0);
    winsockReady = true;
#endif
    const bool closed = strcmp(mode, "closed-port") == 0;
    listener = Bind(&endpoint.port, !closed);
    CHECK(listener != INVALID_TEST_SOCKET);
    if (!strcmp(mode, "deadline")) endpoint.timeoutMs = 200U;
    /* A reserved local port can take several seconds to surface the native
     * refusal. Keep this check distinct from the explicit 200 ms deadline. */
    if (closed) endpoint.timeoutMs = 5000U;
    CHECK(UmiWebFastCgiLoopbackConnectorInit(&endpoint, &connector) == UMI_STATUS_OK);
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (closed) {
        first.context = &endpoint;
        const UmiStatus refused = connector.open(connector.context, cancel, &first);
        if (refused != UMI_STATUS_UNAVAILABLE) fprintf(stderr, "Native open returned status %d\n", (int)refused);
        CHECK(refused == UMI_STATUS_UNAVAILABLE);
        CHECK(first.context == &endpoint && first.read == NULL && first.write == NULL);
        first.context = NULL;
        goto finish;
    }
    CHECK(connector.open(connector.context, cancel, &first) == UMI_STATUS_OK);
    CHECK(first.context != NULL && first.read != NULL && first.write != NULL);
    peer = accept(listener, NULL, NULL);
    CHECK(peer != INVALID_TEST_SOCKET);

    unsigned char bytes[3] = {0};
    size_t count = 77U;
    if (!strcmp(mode, "deadline")) {
        CHECK(first.read(first.context, bytes, sizeof bytes, &count) == UMI_STATUS_TIMEOUT);
        CHECK(count == 0U);
        CHECK(first.write(first.context, "x", 1U, &count) == UMI_STATUS_TIMEOUT);
        CHECK(count == 0U);
    } else if (!strcmp(mode, "cancel-connected")) {
        umi_cancellation_token_request(cancel);
        CHECK(first.read(first.context, bytes, sizeof bytes, &count) == UMI_STATUS_CANCELLED);
        CHECK(count == 0U);
        count = 77U;
        CHECK(first.write(first.context, "x", 1U, &count) == UMI_STATUS_CANCELLED);
        CHECK(count == 0U);
    } else if (!strcmp(mode, "independent")) {
        CHECK(connector.open(connector.context, cancel, &second) == UMI_STATUS_OK);
        CHECK(first.context != second.context);
        connector.close(connector.context, &second);
        CHECK(second.context == NULL && second.read == NULL && second.write == NULL);
        CHECK(first.write(first.context, "x", 1U, &count) == UMI_STATUS_OK && count == 1U);
        CHECK(recv(peer, (char *)bytes, 1, 0) == 1 && bytes[0] == 'x');
    } else if (!strcmp(mode, "binary")) {
        const unsigned char body[3] = {'a', 0U, 'b'};
        CHECK(send(peer, (const char *)body, sizeof body, 0) == (int)sizeof body);
        size_t offset = 0U;
        while (offset < sizeof bytes) {
            CHECK(first.read(first.context, bytes + offset, sizeof bytes - offset, &count) == UMI_STATUS_OK);
            CHECK(count != 0U && count <= sizeof bytes - offset);
            offset += count;
        }
        CHECK(memcmp(bytes, body, sizeof body) == 0);
        CloseTestSocket(peer);
        peer = INVALID_TEST_SOCKET;
        CHECK(first.read(first.context, bytes, sizeof bytes, &count) == UMI_STATUS_UNAVAILABLE && count == 0U);
    } else {
        failed = 2;
    }
    connector.close(connector.context, &first);
    CHECK(first.context == NULL && first.read == NULL && first.write == NULL);
finish:
    if (connector.close != NULL) {
        connector.close(connector.context, &second);
        connector.close(connector.context, &first);
    }
    if (peer != INVALID_TEST_SOCKET) CloseTestSocket(peer);
    if (listener != INVALID_TEST_SOCKET) CloseTestSocket(listener);
#ifdef _WIN32
    if (winsockReady) (void)WSACleanup();
#endif
    umi_cancellation_token_destroy(cancel);
    return failed;
}
