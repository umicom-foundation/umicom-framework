/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/fastcgi/test_connector_route.c
 *
 * PURPOSE:
 *   Verify HTTP dispatch and worker connection lifetime through the native web service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/fastcgi_application.h"
#include "umicom/web/service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); exit(EXIT_FAILURE); \
} } while (0)

typedef struct Fixture {
    const char *mode;
    const char *request;
    size_t received, sent, workerRead, workerWritten;
    size_t opens, closes;
    bool active;
    UmiCancellationToken *cancel;
    char reply[1024];
} Fixture;

static UmiStatus ReadClient(void *context, void *bytes, size_t capacity, size_t *received)
{
    Fixture *fixture = context;
    size_t count = strlen(fixture->request) - fixture->received;
    if (count == 0U) return UMI_STATUS_UNAVAILABLE;
    if (count > capacity) count = capacity;
    if (count > 16U) count = 16U;
    memcpy(bytes, fixture->request + fixture->received, count);
    fixture->received += count;
    *received = count;
    return UMI_STATUS_OK;
}

static UmiStatus WriteClient(void *context, const void *bytes, size_t length, size_t *written)
{
    Fixture *fixture = context;
    size_t count = length < 9U ? length : 9U;
    CHECK(count < sizeof(fixture->reply) - fixture->sent);
    memcpy(fixture->reply + fixture->sent, bytes, count);
    fixture->sent += count;
    fixture->reply[fixture->sent] = '\0';
    *written = count;
    return UMI_STATUS_OK;
}

static UmiStatus ReadWorker(void *context, void *bytes, size_t capacity, size_t *received)
{
    Fixture *fixture = context;
    CHECK(fixture->active && fixture->workerWritten != 0U);
    unsigned char reply[] =
        "\x01\x06\x00\x09\x00\x1c\x00\x00" "Content-Type: text/plain\n\n42"
        "\x01\x03\x00\x09\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    if (strcmp(fixture->mode, "worker-output") == 0) reply[8] = ' ';
    size_t count = sizeof(reply) - 1U - fixture->workerRead;
    if (count == 0U) return UMI_STATUS_UNAVAILABLE;
    if (count > capacity) count = capacity;
    if (count > 5U) count = 5U;
    memcpy(bytes, reply + fixture->workerRead, count);
    fixture->workerRead += count;
    *received = count;
    return UMI_STATUS_OK;
}

static UmiStatus WriteWorker(void *context, const void *bytes, size_t length, size_t *written)
{
    Fixture *fixture = context;
    CHECK(fixture->active && bytes != NULL && length != 0U);
    if (strcmp(fixture->mode, "worker-write") == 0) return UMI_STATUS_IO_ERROR;
    size_t count = length < 11U ? length : 11U;
    fixture->workerWritten += count;
    *written = count;
    return UMI_STATUS_OK;
}

static UmiStatus OpenWorker(void *context, const UmiCancellationToken *token, UmiWebConnectionIo *out)
{
    Fixture *fixture = context;
    CHECK(!fixture->active && token == fixture->cancel && out->context == NULL);
    ++fixture->opens;
    if (strcmp(fixture->mode, "missing-worker") == 0) return UMI_STATUS_NOT_FOUND;
    fixture->active = true;
    fixture->workerRead = 0U;
    fixture->workerWritten = 0U;
    *out = (UmiWebConnectionIo){.context = fixture, .read = ReadWorker, .write = WriteWorker};
    if (strcmp(fixture->mode, "invalid-io") == 0) out->write = NULL;
    if (strcmp(fixture->mode, "cancel-open") == 0) umi_cancellation_token_request(fixture->cancel);
    return UMI_STATUS_OK;
}

static void CloseWorker(void *context, UmiWebConnectionIo *connection)
{
    Fixture *fixture = context;
    CHECK(fixture->active && connection->context == fixture);
    fixture->active = false;
    ++fixture->closes;
}

static UmiStatus Refuse(const UmiWebRequest *request, UmiWebResponse *response, bool *allowed, void *context)
{
    (void)request;
    (void)context;
    *allowed = false;
    return umi_web_response_set_text(response, 403, "text/plain", "Fixture access denied");
}

static UmiStatus StaticPage(const UmiWebRequest *request, UmiWebResponse *response, void *context)
{
    (void)request;
    (void)context;
    return umi_web_response_set_text(response, 200, "text/plain", "Static route");
}

typedef struct Case {
    const char *name;
    UmiStatus status;
    int httpStatus;
    size_t opens, closes;
} Case;

int main(int argc, char **argv)
{
    static const Case cases[] = {
        {"served", UMI_STATUS_OK, 200, 1U, 1U},
        {"repeat", UMI_STATUS_OK, 200, 2U, 2U},
        {"wrong-route", UMI_STATUS_OK, 404, 0U, 0U},
        {"head", UMI_STATUS_OK, 501, 0U, 0U},
        {"request-gate", UMI_STATUS_OK, 403, 0U, 0U},
        {"static-first", UMI_STATUS_OK, 200, 0U, 0U},
        {"missing-worker", UMI_STATUS_NOT_FOUND, 500, 1U, 0U},
        {"invalid-io", UMI_STATUS_INVALID_STATE, 500, 1U, 1U},
        {"worker-write", UMI_STATUS_IO_ERROR, 500, 1U, 1U},
        {"worker-output", UMI_STATUS_PARSE_ERROR, 500, 1U, 1U},
        {"cancel-before", UMI_STATUS_CANCELLED, 500, 0U, 0U},
        {"cancel-open", UMI_STATUS_CANCELLED, 500, 1U, 1U},
    };
    CHECK(argc == 2);
    const Case *expected = NULL;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(argv[1], cases[i].name) == 0) expected = &cases[i];
    CHECK(expected != NULL);
    Fixture fixture = {.mode = expected->name,
        .request = "GET /journal HTTP/1.1\r\nHost: fixture.example\r\n\r\n"};
    if (strcmp(fixture.mode, "wrong-route") == 0)
        fixture.request = "GET /elsewhere HTTP/1.1\r\nHost: fixture.example\r\n\r\n";
    if (strcmp(fixture.mode, "head") == 0)
        fixture.request = "HEAD /journal HTTP/1.1\r\nHost: fixture.example\r\n\r\n";
    CHECK(umi_cancellation_token_create(&fixture.cancel) == UMI_STATUS_OK);
    if (strcmp(fixture.mode, "cancel-before") == 0) umi_cancellation_token_request(fixture.cancel);

    UmiWebService *service = NULL;
    CHECK(umi_web_service_create(&service) == UMI_STATUS_OK);
    UmiWebFastCgiRoute route = {
#ifdef _WIN32
        .application = {"C:/umicom-fixture/journal.php", "/journal", "localhost", 8080U, 9U, 256U},
#else
        .application = {"/srv/umicom-fixture/journal.php", "/journal", "localhost", 8080U, 9U, 256U},
#endif
        .connector = {&fixture, OpenWorker, CloseWorker},
        .cancel = fixture.cancel,
    };
    CHECK(UmiWebServiceSetBodyFallback(service, UmiWebFastCgiRouteHandle, &route) == UMI_STATUS_OK);
    if (strcmp(fixture.mode, "request-gate") == 0)
        CHECK(UmiWebServiceSetRequestGate(service, Refuse, NULL) == UMI_STATUS_OK);
    if (strcmp(fixture.mode, "static-first") == 0) {
        UmiWebRoute httpRoute;
        CHECK(umi_web_route_init(&httpRoute, UMI_HTTP_METHOD_GET, "/journal", StaticPage, NULL) == UMI_STATUS_OK);
        CHECK(umi_web_router_add(umi_web_service_router(service), &httpRoute) == UMI_STATUS_OK);
    }
    const size_t repetitions = strcmp(fixture.mode, "repeat") == 0 ? 2U : 1U;
    const UmiWebConnectionIo client = {.context = &fixture, .read = ReadClient, .write = WriteClient};
    for (size_t i = 0U; i < repetitions; ++i) {
        fixture.received = fixture.sent = 0U;
        UmiWebExchangeResult result;
        CHECK(UmiWebConnectionProcess(service, &client, 1024U, NULL, &result) == expected->status);
        CHECK(result.dispatched && result.response_complete && result.response_status == expected->httpStatus);
        if (expected->status == UMI_STATUS_OK && expected->opens != 0U)
            CHECK(strstr(fixture.reply, "\r\n\r\n42") != NULL);
        if (strcmp(fixture.mode, "static-first") == 0) CHECK(strstr(fixture.reply, "Static route") != NULL);
        if (strcmp(fixture.mode, "head") == 0) CHECK(strstr(fixture.reply, "Method not supported") == NULL);
    }
    CHECK(fixture.opens == expected->opens && fixture.closes == expected->closes && !fixture.active);
    if (strcmp(fixture.mode, "cancel-open") == 0) CHECK(fixture.workerWritten == 0U);
    umi_web_service_destroy(service);
    umi_cancellation_token_destroy(fixture.cancel);
    return EXIT_SUCCESS;
}
