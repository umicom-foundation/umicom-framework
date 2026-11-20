/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/fastcgi/test_application.c
 *
 * PURPOSE:
 *   Trace a bounded HTTP request through a fixed application entry point and verify the exact worker parameters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/fastcgi_application.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); exit(EXIT_FAILURE); \
} } while (0)

#if defined(_WIN32)
#define APP_FILE "C:/Umicom Fixture/private/application.php"
#else
#define APP_FILE "/srv/umicom-fixture/private/application.php"
#endif

typedef struct Worker {
    unsigned char request[4096];
    size_t written;
    size_t consumed;
    int failApplication;
    int malformedOutput;
} Worker;

static UmiStatus Write(void *state, const void *data, size_t length, size_t *written)
{
    Worker *worker = state;
    const size_t count = length < 11U ? length : 11U;
    CHECK(count <= sizeof worker->request - worker->written);
    memcpy(worker->request + worker->written, data, count);
    worker->written += count;
    *written = count;
    return UMI_STATUS_OK;
}

static UmiStatus Read(void *state, void *data, size_t capacity, size_t *received)
{
    Worker *worker = state;
    unsigned char reply[] =
        "\x01\x06\x00\x09\x00\x1c\x00\x00" "Content-Type: text/plain\n\n42"
        "\x01\x03\x00\x09\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    const size_t wireBytes = sizeof reply - 1U;
    if (worker->failApplication) reply[wireBytes - 5U] = 1U;
    if (worker->malformedOutput) reply[8U] = ' ';
    if (worker->consumed == wireBytes) {
        *received = 0U;
        return UMI_STATUS_OK;
    }
    size_t count = wireBytes - worker->consumed;
    if (count > capacity) count = capacity;
    if (count > 7U) count = 7U;
    memcpy(data, reply + worker->consumed, count);
    worker->consumed += count;
    *received = count;
    return UMI_STATUS_OK;
}

static void CheckParameters(const Worker *worker, bool post)
{
    const char *const expected[][2] = {
        {"GATEWAY_INTERFACE", "CGI/1.1"}, {"SERVER_PROTOCOL", "HTTP/1.1"},
        {"SERVER_SOFTWARE", "Umicom"}, {"SERVER_NAME", "localhost"}, {"SERVER_PORT", "8443"},
        {"REQUEST_METHOD", post ? "POST" : "GET"},
        {"SCRIPT_FILENAME", APP_FILE}, {"SCRIPT_NAME", "/reports/current"},
        {"QUERY_STRING", post ? "format=csv%20data" : ""},
        {"REQUEST_URI", post ? "/reports/current?format=csv%20data" : "/reports/current"},
        {"CONTENT_LENGTH", post ? "3" : "0"}, {"CONTENT_TYPE", "application/octet-stream"}
    };
    size_t parameter = 0U, begin = 0U, stdinRecords = 0U;
    bool paramsClosed = false;
    while (begin < worker->written) {
        UmiWebFastCgiRecord record;
        CHECK(UmiWebFastCgiParseRecord(worker->request + begin, worker->written - begin, &record) == UMI_STATUS_OK);
        CHECK(record.requestId == 9U);
        if (begin == 0U) {
            CHECK(record.type == UMI_WEB_FASTCGI_BEGIN_REQUEST);
        } else if (record.type == UMI_WEB_FASTCGI_PARAMS && record.bodyBytes == 0U) {
            CHECK(!paramsClosed && parameter == (post ? 12U : 11U));
            paramsClosed = true;
        } else if (record.type == UMI_WEB_FASTCGI_PARAMS) {
            CHECK(!paramsClosed && parameter < (post ? 12U : 11U) && record.bodyBytes >= 2U);
            const size_t name = record.body[0], value = record.body[1];
            CHECK(name == strlen(expected[parameter][0]) && value == strlen(expected[parameter][1]));
            CHECK(record.bodyBytes == 2U + name + value);
            CHECK(memcmp(record.body + 2U, expected[parameter][0], name) == 0);
            CHECK(memcmp(record.body + 2U + name, expected[parameter][1], value) == 0);
            ++parameter;
        } else {
            CHECK(paramsClosed && record.type == UMI_WEB_FASTCGI_STDIN);
            if (post && stdinRecords == 0U)
                CHECK(record.bodyBytes == 3U && memcmp(record.body, "a\0b", 3U) == 0);
            else
                CHECK(record.bodyBytes == 0U);
            ++stdinRecords;
        }
        begin += record.consumed;
    }
    CHECK(paramsClosed && stdinRecords == (post ? 2U : 1U));
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    const bool post = strcmp(name, "get") != 0;
    UmiWebRequest *request = malloc(sizeof *request);
    UmiWebResponse *response = calloc(1U, sizeof *response);
    CHECK(request != NULL && response != NULL);
    const char get[] = "GET /reports/current HTTP/1.1\r\nHost: request.example\r\n\r\n";
    const char send[] = "POST /reports/current?format=csv%20data HTTP/1.1\r\nHost: request.example\r\n"
        "Cookie: fixture-only=1\r\nAuthorization: Fixture placeholder\r\n"
        "Content-Type: application/octet-stream\r\nContent-Length: 3\r\n\r\na\0b";
    const void *raw = post ? send : get;
    const size_t bytes = post ? sizeof send - 1U : sizeof get - 1U;
    CHECK(UmiWebRequestParseStrict(raw, bytes, 16384U, request) == UMI_STATUS_OK);
    UmiWebFastCgiApplication config = {.scriptFilename = APP_FILE, .scriptName = "/reports/current",
        .serverName = "localhost", .serverPort = 8443U, .requestId = 9U, .replyLimit = 512U};
    Worker worker = {0};
    const UmiWebConnectionIo io = {.context = &worker, .write = Write, .read = Read};
    UmiWebResponseBody *body = NULL;
    UmiStatus expected = UMI_STATUS_OK;
    bool expectWrites = false;
    response->status = 418;
    UmiCancellationToken *cancel = NULL;

    if (strcmp(name, "post") == 0 || strcmp(name, "get") == 0) {
        expectWrites = true;
    } else if (strcmp(name, "other-path") == 0) {
        strcpy(request->path, "/reports/other");
        expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(name, "relative-script") == 0) {
        config.scriptFilename = "private/application.php";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "server-name") == 0) {
        config.serverName = "localhost:8443";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "head") == 0) {
        request->method = UMI_HTTP_METHOD_HEAD;
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    } else if (strcmp(name, "duplicate-content-type") == 0) {
        CHECK(umi_web_header_set(&request->headers[request->header_count], "content-type",
            "text/plain") == UMI_STATUS_OK);
        ++request->header_count;
        expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(name, "malformed-query") == 0) {
        strcpy(request->query, "space inside");
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "body-budget") == 0) {
        request->body_length = sizeof request->body + 1U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "reply-budget") == 0) {
        config.replyLimit = UMI_WEB_FASTCGI_EXCHANGE_LIMIT + 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "cancel") == 0) {
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    } else if (strcmp(name, "worker-exit") == 0) {
        worker.failApplication = 1;
        expectWrites = true;
        expected = UMI_STATUS_INVALID_STATE;
    } else {
        CHECK(strcmp(name, "worker-document") == 0);
        worker.malformedOutput = 1;
        expectWrites = true;
        expected = UMI_STATUS_PARSE_ERROR;
    }

    CHECK(UmiWebFastCgiApplicationHandle(&config, request, &io, cancel, response, &body) == expected);
    CHECK((worker.written != 0U) == expectWrites);
    if (expectWrites) CheckParameters(&worker, post);
    if (expected == UMI_STATUS_OK) {
        CHECK(response->status == 200 && response->header_count == 1U && response->body_length == 0U);
        CHECK(UmiWebResponseBodyLength(body) == 2U && memcmp(UmiWebResponseBodyBytes(body), "42", 2U) == 0);
    } else {
        CHECK(response->status == 418 && response->header_count == 0U && body == NULL);
    }
    UmiWebResponseBodyDestroy(body);
    umi_cancellation_token_destroy(cancel);
    free(response);
    free(request);
    return EXIT_SUCCESS;
}
