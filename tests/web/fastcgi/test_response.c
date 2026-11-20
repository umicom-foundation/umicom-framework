/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/fastcgi/test_response.c
 *
 * PURPOSE:
 *   Verify worker-output admission, atomic ownership and HTTP framing with local fixtures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/fastcgi_response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); exit(EXIT_FAILURE); \
} } while (0)

typedef struct Document {
    const char *name;
    const char *data;
    size_t length;
    UmiStatus expected;
    int code;
    size_t bodyBytes;
} Document;

#define DOCUMENT(name, data, code, bytes) {name, data, sizeof(data) - 1U, UMI_STATUS_OK, code, bytes}
#define REFUSAL(name, data) {name, data, sizeof(data) - 1U, UMI_STATUS_PARSE_ERROR, 0, 0U}

static const Document documents[] = {
    DOCUMENT("binary", "Content-Type: application/octet-stream\r\n\r\na\0b", 200, 3U),
    DOCUMENT("unix-lines", "content-type: text/plain\n\n42", 200, 2U),
    DOCUMENT("status-and-length", "sTAtus: 201 Worker phrase\r\nContent-Type: text/plain\r\n"
        "content-length:\t2 \r\n\r\n42", 201, 2U),
    DOCUMENT("repeated-cookie", "Content-Type: text/plain\r\nSet-Cookie: a=1\r\n"
        "set-cookie: b=2\r\n\r\n42", 200, 2U),
    DOCUMENT("empty", "Status: 204\r\n\r\n", 204, 0U),
    DOCUMENT("location", "Location: /result\nStatus: 303 See Other\n\n", 303, 0U),
    REFUSAL("implicit-location", "Location: /result\n\n"),
    REFUSAL("mixed-lines", "Content-Type: text/plain\r\nX-Worker: test\n\n42"),
    REFUSAL("unterminated", "Content-Type: text/plain\r\n42"),
    REFUSAL("bare-carriage-return", "Content-Type: text/plain\rdata\n\n42"),
    REFUSAL("folded-field", "Content-Type: text/plain\r\n next line\r\n\r\n42"),
    REFUSAL("nul-header", "Content-Type: text\0/plain\r\n\r\n42"),
    REFUSAL("missing-type", "X-Worker: test\n\n42"),
    REFUSAL("duplicate-type", "Content-Type: text/plain\ncontent-type: text/plain\n\n42"),
    REFUSAL("duplicate-status", "Status: 204\nstatus: 204\n\n"),
    REFUSAL("duplicate-length", "Content-Type: text/plain\nContent-Length: 2\ncontent-length: 2\n\n42"),
    REFUSAL("wrong-length", "Content-Type: text/plain\nContent-Length: 3\n\n42"),
    REFUSAL("length-list", "Content-Type: text/plain\nContent-Length: 2, 2\n\n42"),
    REFUSAL("invalid-status", "Status: 201Created\nContent-Type: text/plain\n\n42"),
    REFUSAL("interim-status", "Status: 103 Early Hints\nContent-Type: text/plain\n\n42"),
    REFUSAL("body-forbidden", "Status: 304\nContent-Type: text/plain\n\n42"),
    REFUSAL("reset-with-body", "Status: 205\nContent-Type: text/plain\n\n42"),
    REFUSAL("trailer", "Content-Type: text/plain\nTrailer: X-Worker\n\n42"),
    REFUSAL("connection-framing", "Content-Type: text/plain\r\nCoNNection: close\r\n\r\n42"),
    REFUSAL("transfer-framing", "Content-Type: text/plain\r\nTransfer-Encoding: chunked\r\n\r\n42")
};

static UmiWebFastCgiResult Result(const char *data, size_t length)
{
    UmiWebFastCgiResult result = {
        .reply = {.complete = 1, .stdoutBytes = length, .protocolStatus = UMI_WEB_FASTCGI_REQUEST_COMPLETE},
        .body = malloc(length),
        .bodyBytes = length
    };
    CHECK(result.body != NULL);
    memcpy(result.body, data, length);
    return result;
}

static void CheckWire(const UmiWebResponse *response, const UmiWebResponseBody *body)
{
    char wire[4096];
    size_t written = 0U;
    CHECK(UmiWebResponseFormatClosedHeaders(response, UmiWebResponseBodyLength(body),
        wire, sizeof(wire) - 1U, &written) == UMI_STATUS_OK);
    wire[written] = '\0';
    CHECK(strstr(wire, "Connection: close\r\n\r\n") != NULL);
    CHECK(strstr(wire, "Worker phrase") == NULL && strstr(wire, "sTAtus:") == NULL);
    if (response->status == 201 && body != NULL) {
        CHECK(strstr(wire, "201 Created\r\n") != NULL);
        const char *first = strstr(wire, "Content-Length: 2\r\n");
        CHECK(first != NULL && strstr(first + 1, "Content-Length:") == NULL);
        CHECK(strstr(wire, "content-length:") == NULL);
    }
    if (response->status == 204)
        CHECK(strstr(wire, "Content-Length:") == NULL);
    if (response->status == 303)
        CHECK(strstr(wire, "303 See Other\r\nLocation: /result\r\n") != NULL);
}

static void RunDocument(const Document *document, UmiWebResponse *response)
{
    UmiWebFastCgiResult result = Result(document->data, document->length);
    UmiWebResponseBody *body = NULL;
    CHECK(UmiWebFastCgiResponseDecode(&result, response, &body) == document->expected);
    UmiWebFastCgiResultDestroy(&result);
    if (document->expected != UMI_STATUS_OK) {
        CHECK(response->status == 418 && response->header_count == 0U && body == NULL);
        return;
    }
    CHECK(response->status == document->code && response->body_length == 0U);
    CHECK(UmiWebResponseBodyLength(body) == document->bodyBytes);
    if (document->bodyBytes != 0U)
        CHECK(memcmp(UmiWebResponseBodyBytes(body),
            document->data + document->length - document->bodyBytes, document->bodyBytes) == 0);
    if (strcmp(document->name, "repeated-cookie") == 0) {
        CHECK(response->header_count == 3U);
        CHECK(strcmp(response->headers[1].value, "a=1") == 0);
        CHECK(strcmp(response->headers[2].value, "b=2") == 0);
    }
    CheckWire(response, body);
    UmiWebResponseBodyDestroy(body);
}

static void CheckCapacities(const char *name, UmiWebResponse *response)
{
    const size_t cap = UMI_WEB_FASTCGI_HEADER_LIMIT;
    UmiWebResponseBody *body = NULL;
    char *input = malloc(cap);
    CHECK(input != NULL);
    size_t bytes = 0U;
    if (strcmp(name, "header-budget") == 0) {
        memset(input, 'a', cap);
        bytes = cap;
    } else if (strcmp(name, "field-budget") == 0) {
        for (size_t i = 0U; i <= UMI_WEB_MAX_HEADERS; ++i) {
            memcpy(input + bytes, "X: test\n", 8U);
            bytes += 8U;
        }
        input[bytes++] = '\n';
    } else if (strcmp(name, "value-budget") == 0) {
        const char type[] = "Content-Type: ";
        memcpy(input, type, sizeof(type) - 1U);
        memset(input + sizeof(type) - 1U, 'a', UMI_WEB_HEADER_VALUE_CAPACITY);
        bytes = sizeof(type) - 1U + UMI_WEB_HEADER_VALUE_CAPACITY;
        memcpy(input + bytes, "\n\n", 2U);
        bytes += 2U;
    } else {
        CHECK(strcmp(name, "length-budget") == 0);
        const char header[] = "Content-Length: 9999999999999999999999999\n\n";
        bytes = sizeof(header) - 1U;
        memcpy(input, header, bytes);
    }
    UmiWebFastCgiResult result = Result(input, bytes);
    free(input);
    CHECK(UmiWebFastCgiResponseDecode(&result, response, &body) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(response->status == 418 && body == NULL);
    UmiWebFastCgiResultDestroy(&result);
}

typedef struct Worker {
    size_t sent;
    size_t received;
} Worker;

static UmiStatus Write(void *state, const void *data, size_t length, size_t *written)
{
    Worker *worker = state;
    CHECK(data != NULL && length != 0U);
    ++worker->sent;
    *written = 1U;
    return UMI_STATUS_OK;
}

static UmiStatus Read(void *state, void *buffer, size_t capacity, size_t *received)
{
    Worker *worker = state;
    /* Consecutive stdout records split "Content-Type", with transport reads
     * split again. The formatter sees a contiguous document only after END. */
    static const unsigned char wire[] =
        "\x01\x06\x00\x05\x00\x08\x00\x00" "Content-"
        "\x01\x06\x00\x05\x00\x16\x00\x00" "Type: text/plain\r\n\r\n42"
        "\x01\x07\x00\x05\x00\x01\x00\x00" "!"
        "\x01\x03\x00\x05\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    CHECK(worker->sent == 32U && capacity != 0U && worker->received < sizeof(wire) - 1U);
    *(unsigned char *)buffer = wire[worker->received++];
    *received = 1U;
    return UMI_STATUS_OK;
}

static void RunExchange(UmiWebResponse *response)
{
    Worker worker = {0};
    UmiWebConnectionIo io = {.context = &worker, .read = Read, .write = Write};
    UmiWebFastCgiRequest request = {.requestId = 5U, .replyLimit = 512U};
    UmiWebFastCgiResult result = {0};
    CHECK(UmiWebFastCgiExchange(&io, &request, NULL, &result) == UMI_STATUS_OK);
    CHECK(result.reply.stderrBytes == 1U);
    UmiWebResponseBody *body = NULL;
    CHECK(UmiWebFastCgiResponseDecode(&result, response, &body) == UMI_STATUS_OK);
    UmiWebFastCgiResultDestroy(&result);
    CHECK(response->status == 200 && response->header_count == 1U);
    CHECK(UmiWebResponseBodyLength(body) == 2U && memcmp(UmiWebResponseBodyBytes(body), "42", 2U) == 0);
    CheckWire(response, body);
    UmiWebResponseBodyDestroy(body);
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiWebResponse *response = calloc(1U, sizeof *response);
    CHECK(response != NULL);
    response->status = 418;
    const char *name = argv[1];
    bool known = false;
    for (size_t i = 0U; i < sizeof(documents) / sizeof(documents[0]); ++i) {
        if (strcmp(name, documents[i].name) == 0) {
            RunDocument(&documents[i], response);
            known = true;
            break;
        }
    }
    if (!known && strcmp(name, "actual-exchange") == 0) {
        RunExchange(response);
    } else if (!known && strcmp(name, "failed-worker") == 0) {
        UmiWebFastCgiResult result = Result(documents[0].data, documents[0].length);
        UmiWebResponseBody *body = NULL;
        result.reply.applicationStatus = 1U;
        CHECK(UmiWebFastCgiResponseDecode(&result, response, &body) == UMI_STATUS_INVALID_STATE);
        result.reply.applicationStatus = 0U;
        result.reply.complete = 0;
        CHECK(UmiWebFastCgiResponseDecode(&result, response, &body) == UMI_STATUS_INVALID_STATE);
        result.reply.complete = 1;
        result.reply.stdoutBytes -= 1U;
        CHECK(UmiWebFastCgiResponseDecode(&result, response, &body) == UMI_STATUS_INVALID_STATE);
        CHECK(response->status == 418 && body == NULL);
        UmiWebFastCgiResultDestroy(&result);
    } else if (!known) {
        CheckCapacities(name, response);
    }
    free(response);
    return EXIT_SUCCESS;
}
