/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_health.c
 *
 * PURPOSE:
 *   Verify explicit probe admission and real bounded loopback GET behavior with an inert native server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/local_models/manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
typedef SOCKET HealthSocket;
#define BAD_SOCKET INVALID_SOCKET
#define CloseSocket closesocket
static void Delay(unsigned milliseconds) { Sleep(milliseconds); }
#else
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
typedef int HealthSocket;
#define BAD_SOCKET (-1)
#define CloseSocket close
static void Delay(unsigned milliseconds)
{
    struct timespec pause = {(time_t)(milliseconds / 1000U), (long)(milliseconds % 1000U) * 1000000L};
    (void)nanosleep(&pause, NULL);
}
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
typedef struct HealthServer {
    HealthSocket listener;
    const char *scenario;
    UmiAiWorkspaceCancellation *token;
    int requests;
    int error;
} HealthServer;

static int WaitReadable(HealthSocket socket)
{
    fd_set readable; FD_ZERO(&readable); FD_SET(socket, &readable);
    struct timeval wait = {5, 0};
#ifdef _WIN32
    return select(0, &readable, NULL, NULL, &wait);
#else
    return select(socket + 1, &readable, NULL, NULL, &wait);
#endif
}
static int SendAll(HealthSocket socket, const char *bytes, size_t size)
{
    while (size > 0U) {
#ifdef _WIN32
        int count = send(socket, bytes, (int)size, 0);
#else
        ssize_t count = send(socket, bytes, size, 0);
#endif
        if (count <= 0) return 0;
        bytes += (size_t)count; size -= (size_t)count;
    }
    return 1;
}
/* Sequence selection belongs to the inert fixture, not the production wait.
 * Each accepted request gets an explicit response from this small script. */
static const char *ReadinessReply(const HealthServer *server)
{
    const char *name = server->scenario;
    if (!strcmp(name, "wait-loading-ready")) return server->requests == 1 ? "loading" : "ready";
    if (!strcmp(name, "wait-loading-limit") || !strcmp(name, "wait-cli-limit") ||
        !strcmp(name, "wait-cancel-after-response")) return "loading";
    if (!strcmp(name, "wait-disconnect-ready")) return server->requests == 1 ? "disconnect" : "ready";
    if (!strcmp(name, "wait-disconnect-limit")) return "disconnect";
    if (!strcmp(name, "wait-stale")) return server->requests == 1 ? "loading" : "bad-json";
    if (!strcmp(name, "wait-bad-json")) return "bad-json";
    if (!strcmp(name, "wait-oversized")) return "oversized";
    if (!strcmp(name, "wait-redirect")) return "redirect";
    if (!strcmp(name, "wait-unauthorized")) return "unauthorized";
    if (!strcmp(name, "wait-cancel")) return "cancel";
    return name;
}
static int ReadinessRequests(const char *name)
{
    if (!strcmp(name, "wait-loading-ready") || !strcmp(name, "wait-disconnect-ready") ||
        !strcmp(name, "wait-stale")) return 2;
    if (!strcmp(name, "wait-loading-limit") || !strcmp(name, "wait-disconnect-limit") ||
        !strcmp(name, "wait-redirect") || !strcmp(name, "wait-unauthorized") ||
        !strcmp(name, "wait-cli-limit")) return 3;
    return 1;
}

static void ServeHealth(HealthServer *server)
{
    if (WaitReadable(server->listener) != 1) { server->error = 1; return; }
    HealthSocket client = accept(server->listener, NULL, NULL);
    if (client == BAD_SOCKET) { server->error = 2; return; }
    char request[2048]; size_t used = 0U; request[0] = '\0';
    while (strstr(request, "\r\n\r\n") == NULL) {
        if (used == sizeof request - 1U || WaitReadable(client) != 1) { server->error = 3; goto done; }
#ifdef _WIN32
        int count = recv(client, request + used, (int)(sizeof request - 1U - used), 0);
#else
        ssize_t count = recv(client, request + used, sizeof request - 1U - used, 0);
#endif
        if (count <= 0) { server->error = 4; goto done; }
        used += (size_t)count; request[used] = '\0';
    }
    ++server->requests;
    if (strncmp(request, "GET /health HTTP/1.1\r\n", 22U) != 0 ||
        strstr(request, "Authorization:") != NULL || strstr(request, "Cookie:") != NULL ||
        strstr(request, "Transfer-Encoding:") != NULL || strstr(request, "Content-Length:") != NULL) {
        server->error = 5; goto done;
    }
    const char *reply = ReadinessReply(server);
    if (!strcmp(reply, "disconnect")) goto done;
    if (!strcmp(reply, "cancel")) {
        UmiAiWorkspaceCancellationRequest(server->token);
        Delay(1200U);
    } else if (!strcmp(reply, "timeout")) Delay(600U);
    const char *body = "{\"status\":\"ok\"}";
    char oversized[4097];
    int code = 200;
    if (!strcmp(reply, "loading")) {
        code = 503;
        body = "{\"error\":{\"code\":503,\"message\":\"Loading model\",\"type\":\"unavailable_error\"}}";
    } else if (!strcmp(reply, "bad-json")) body = "{";
    else if (!strcmp(reply, "oversized")) {
        memset(oversized, ' ', sizeof oversized - 1U);
        oversized[sizeof oversized - 1U] = '\0'; body = oversized;
    } else if (!strcmp(reply, "redirect")) code = 302;
    else if (!strcmp(reply, "unauthorized")) code = 401;
    char headers[256];
    int headerBytes = snprintf(headers, sizeof headers,
        "HTTP/1.1 %d Fixture\r\nContent-Type: application/json\r\nContent-Length: %zu\r\nConnection: close\r\n%s\r\n",
        code, strlen(body), code == 302 ? "Location: http://127.0.0.1:1/do-not-follow\r\n" : "");
    if (headerBytes <= 0 || (size_t)headerBytes >= sizeof headers) { server->error = 6; goto done; }
    /* Timeout and cancellation deliberately let the client close early. */
    if (!SendAll(client, headers, (size_t)headerBytes) || !SendAll(client, body, strlen(body))) {
        if (strcmp(reply, "timeout") && strcmp(reply, "cancel")) server->error = 7;
    }
done:
    (void)CloseSocket(client);
}
/* The original worker served exactly one health request. It is retained for
 * review; the sequence worker below preserves that default and additionally
 * serves the fixed readiness scripts without loading a model. */
#if 0
#ifdef _WIN32
static DWORD WINAPI HealthWorker(LPVOID context) { ServeHealth(context); return 0U; }
#else
static void *HealthWorker(void *context) { ServeHealth(context); return NULL; }
#endif
#endif
static void ServeSequence(HealthServer *server)
{
    int requests = ReadinessRequests(server->scenario);
    for (int i = 0; i < requests && server->error == 0; ++i) ServeHealth(server);
    if (!strcmp(server->scenario, "wait-cancel-after-response")) {
        /* Cancellation may arrive during response cleanup or the pause.
         * Both must stop the sequence before a second request is attempted. */
        Delay(100U);
        UmiAiWorkspaceCancellationRequest(server->token);
    }
}
#ifdef _WIN32
static DWORD WINAPI HealthWorker(LPVOID context) { ServeSequence(context); return 0U; }
#else
static void *HealthWorker(void *context) { ServeSequence(context); return NULL; }
#endif

static int Admission(const char *scenario)
{
    UmiAiLocalHealth result; unsigned char before[sizeof result];
    memset(&result, 0xa5, sizeof result); memcpy(before, &result, sizeof result);
    if (!strcmp(scenario, "consent")) {
        CHECK(UmiModelHealthProbe(8080U, 1000U, false, NULL, &result) == UMI_STATUS_PERMISSION_DENIED);
    } else if (!strcmp(scenario, "arguments")) {
        CHECK(UmiModelHealthProbe(1U, 1000U, true, NULL, &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelHealthProbe(8080U, 99U, true, NULL, &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelHealthProbe(8080U, 60001U, true, NULL, &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelHealthProbe(8080U, 1000U, true, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(scenario, "cancelled")) {
        UmiAiWorkspaceCancellation *token = NULL;
        CHECK(UmiAiWorkspaceCancellationCreate(&token) == UMI_STATUS_OK);
        UmiAiWorkspaceCancellationRequest(token);
        UmiStatus status = UmiModelHealthProbe(8080U, 1000U, true, token, &result);
        UmiAiWorkspaceCancellationDestroy(token);
        CHECK(status == UMI_STATUS_CANCELLED);
    } else if (!strcmp(scenario, "preview")) {
        char *arguments[] = {"health", "8080", "1000"};
        CHECK(UmiModelsCommand(3, arguments) == 0);
        char *invalid[] = {"health", "8080", "1000", "--apply"};
        CHECK(UmiModelsCommand(4, invalid) == 2);
    } else return 2;
    CHECK(!memcmp(before, &result, sizeof result));
    return 0;
}
static int ReadinessCall(const char *scenario, uint16_t port,
    const UmiAiWorkspaceCancellation *token)
{
    if (!strcmp(scenario, "wait-cli-ready") || !strcmp(scenario, "wait-cli-limit")) {
        char portText[6]; (void)snprintf(portText, sizeof portText, "%u", (unsigned)port);
        char *args[] = {"wait", portText, "3", "50", "2000", "--probe"};
        CHECK(UmiModelsCommand(6, args) == (!strcmp(scenario, "wait-cli-ready") ? 0 : 1));
        return 0;
    }
    UmiModelReadinessPlan plan;
    uint32_t interval = !strcmp(scenario, "wait-cancel-after-response") ? 500U : 50U;
    CHECK(UmiModelReadinessPlanCreate(port, 3U, interval, 2000U, &plan) == UMI_STATUS_OK);
    UmiModelReadinessReport report;
    memset(&report, 0xa5, sizeof report);
    UmiStatus wanted = UMI_STATUS_OK;
    if (!strcmp(scenario, "wait-loading-limit") || !strcmp(scenario, "wait-disconnect-limit") ||
        !strcmp(scenario, "wait-redirect") || !strcmp(scenario, "wait-unauthorized")) wanted = UMI_STATUS_TIMEOUT;
    else if (!strcmp(scenario, "wait-bad-json") || !strcmp(scenario, "wait-stale")) wanted = UMI_STATUS_PARSE_ERROR;
    else if (!strcmp(scenario, "wait-oversized")) wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    else if (!strcmp(scenario, "wait-cancel") || !strcmp(scenario, "wait-cancel-after-response")) wanted = UMI_STATUS_CANCELLED;
    CHECK(UmiModelReadinessWait(&plan, true, token, &report) == wanted);
    CHECK(report.attempts_completed == (uint32_t)ReadinessRequests(scenario));
    if (wanted == UMI_STATUS_OK) {
        CHECK(report.has_health && report.last_probe_status == UMI_STATUS_OK);
        CHECK(report.last_health.http_status == 200U && report.last_health.endpoint_reports_ready);
    } else if (!strcmp(scenario, "wait-loading-limit")) {
        CHECK(report.has_health && report.last_probe_status == UMI_STATUS_OK);
        CHECK(report.last_health.http_status == 503U && !report.last_health.endpoint_reports_ready);
    } else if (wanted != UMI_STATUS_CANCELLED) {
        CHECK(!report.has_health && report.last_health.http_status == 0U && !report.last_health.endpoint_reports_ready);
        CHECK(report.last_probe_status == (wanted == UMI_STATUS_TIMEOUT ? UMI_STATUS_IO_ERROR : wanted));
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *scenario = argv[1];
    int admission = Admission(scenario);
    if (admission != 2) return admission;
    static const char *const waitCases[] = {
        "wait-ready", "wait-loading-ready", "wait-loading-limit", "wait-disconnect-ready",
        "wait-disconnect-limit", "wait-bad-json", "wait-oversized", "wait-redirect",
        "wait-unauthorized", "wait-cancel", "wait-cancel-after-response", "wait-stale",
        "wait-cli-ready", "wait-cli-limit", "wait-cli-preview"
    };
    bool readiness = false;
    for (size_t i = 0U; i < sizeof waitCases / sizeof waitCases[0]; ++i)
        if (!strcmp(scenario, waitCases[i])) readiness = true;
    UmiStatus expected = UMI_STATUS_OK;
    if (!strcmp(scenario, "bad-json")) expected = UMI_STATUS_PARSE_ERROR;
    else if (!strcmp(scenario, "oversized")) expected = UMI_STATUS_CAPACITY_EXCEEDED;
    else if (!strcmp(scenario, "redirect") || !strcmp(scenario, "unauthorized")) expected = UMI_STATUS_IO_ERROR;
    else if (!strcmp(scenario, "timeout")) expected = UMI_STATUS_TIMEOUT;
    else if (!strcmp(scenario, "cancel")) expected = UMI_STATUS_CANCELLED;
    else if (!readiness && strcmp(scenario, "ready") && strcmp(scenario, "loading") && strcmp(scenario, "proxy-bypass")) return 2;
    if (!UmiAiWorkspaceLocalProviderAvailable()) {
        fputs("SKIP: local HTTP support not built; native loopback health test not executed.\n", stderr);
        return 77;
    }
#ifdef _WIN32
    WSADATA winsock;
    CHECK(WSAStartup(MAKEWORD(2, 2), &winsock) == 0);
#else
    (void)signal(SIGPIPE, SIG_IGN);
#endif
    HealthServer server = {0}; server.scenario = scenario;
    CHECK(UmiAiWorkspaceCancellationCreate(&server.token) == UMI_STATUS_OK);
    server.listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    CHECK(server.listener != BAD_SOCKET);
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET; address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    CHECK(bind(server.listener, (struct sockaddr *)&address, sizeof address) == 0);
    CHECK(listen(server.listener, 1) == 0);
#ifdef _WIN32
    int addressSize = sizeof address;
#else
    socklen_t addressSize = sizeof address;
#endif
    CHECK(getsockname(server.listener, (struct sockaddr *)&address, &addressSize) == 0);
    uint16_t port = ntohs(address.sin_port);
    CHECK(port >= 1024U);
    if (!strcmp(scenario, "wait-cli-preview")) {
        char portText[6]; (void)snprintf(portText, sizeof portText, "%u", (unsigned)port);
        char *args[] = {"wait", portText, "3", "50", "100"};
        int result = UmiModelsCommand(5, args);
        /* An active listener makes accidental loopback traffic observable.
         * Preview must finish without even queuing a connection. */
        fd_set readable; FD_ZERO(&readable); FD_SET(server.listener, &readable);
        struct timeval immediate = {0, 0};
#ifdef _WIN32
        int pending = select(0, &readable, NULL, NULL, &immediate);
#else
        int pending = select(server.listener + 1, &readable, NULL, NULL, &immediate);
#endif
        (void)CloseSocket(server.listener);
        UmiAiWorkspaceCancellationDestroy(server.token);
#ifdef _WIN32
        (void)WSACleanup();
#endif
        CHECK(result == 0 && pending == 0);
        return 0;
    }
    if (!strcmp(scenario, "proxy-bypass")) {
#ifdef _WIN32
        CHECK(SetEnvironmentVariableA("http_proxy", "http://127.0.0.1:1"));
        CHECK(SetEnvironmentVariableA("ALL_PROXY", "http://127.0.0.1:1"));
        CHECK(SetEnvironmentVariableA("NO_PROXY", ""));
#else
        CHECK(setenv("http_proxy", "http://127.0.0.1:1", 1) == 0);
        CHECK(setenv("ALL_PROXY", "http://127.0.0.1:1", 1) == 0);
        CHECK(setenv("NO_PROXY", "", 1) == 0);
        CHECK(setenv("no_proxy", "", 1) == 0);
#endif
    }
#ifdef _WIN32
    HANDLE thread = CreateThread(NULL, 0U, HealthWorker, &server, 0U, NULL);
    CHECK(thread != NULL);
#else
    pthread_t thread;
    CHECK(pthread_create(&thread, NULL, HealthWorker, &server) == 0);
#endif
    UmiAiLocalHealth result; unsigned char before[sizeof result];
    memset(&result, 0xa5, sizeof result); memcpy(before, &result, sizeof result);
    int readinessCheck = readiness ? ReadinessCall(scenario, port, server.token) : 0;
    UmiStatus status = UMI_STATUS_OK;
    if (!readiness) {
        status = UmiModelHealthProbe(port, !strcmp(scenario, "timeout") ? 100U : 4000U,
            true, server.token, &result);
    }
#ifdef _WIN32
    DWORD joined = WaitForSingleObject(thread, 12000U);
    CHECK(joined == WAIT_OBJECT_0); (void)CloseHandle(thread);
#else
    CHECK(pthread_join(thread, NULL) == 0);
#endif
    (void)CloseSocket(server.listener);
    UmiAiWorkspaceCancellationDestroy(server.token);
#ifdef _WIN32
    (void)WSACleanup();
#endif
    CHECK(readinessCheck == 0);
    CHECK(server.requests == ReadinessRequests(scenario) && server.error == 0);
    if (readiness) return 0;
    CHECK(status == expected);
    if (status == UMI_STATUS_OK) {
        CHECK(result.http_status == (!strcmp(scenario, "loading") ? 503U : 200U));
        CHECK(result.endpoint_reports_ready == (strcmp(scenario, "loading") != 0));
    } else CHECK(!memcmp(before, &result, sizeof result));
    return 0;
}
