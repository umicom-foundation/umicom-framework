/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/fastcgi/test_fastcgi.c
 *
 * PURPOSE:
 *   Check bounded FastCGI encoding against independent wire fixtures and
 *   verify reply ownership, completion and refusal behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/fastcgi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); exit(EXIT_FAILURE); \
} } while (0)

/* These header and body bytes are independent of the request encoder. */
static const uint8_t output[] = {1, 6, 0, 5, 0, 3, 1, 0, 'A', 0, 'B', 0xee};
static const uint8_t diagnostic[] = {1, 7, 0, 5, 0, 1, 0, 0, '!'};
static const uint8_t closeOutput[] = {1, 6, 0, 5, 0, 0, 0, 0};
static const uint8_t complete[] = {1, 3, 0, 5, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static void CheckEncoding(const char *name)
{
    uint8_t buffer[64];
    size_t written = 91U;
    memset(buffer, 0x7a, sizeof buffer);
    if (strcmp(name, "begin") == 0) {
        const uint8_t expected[] = {1, 1, 0x12, 0x34, 0, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
        CHECK(UmiWebFastCgiBegin(0x1234U, buffer, sizeof buffer, &written) == UMI_STATUS_OK);
        CHECK(written == sizeof expected && memcmp(buffer, expected, written) == 0);
        return;
    }
    if (strcmp(name, "padded-input") == 0) {
        const uint8_t binary[] = {0x80, 0x00, 0x29};
        const uint8_t expected[] = {1, 5, 0, 5, 0, 3, 5, 0, 0x80, 0x00, 0x29, 0, 0, 0, 0, 0};
        CHECK(UmiWebFastCgiInput(UMI_WEB_FASTCGI_STDIN, 5U, binary, sizeof binary,
            buffer, sizeof buffer, &written) == UMI_STATUS_OK);
        CHECK(written == sizeof expected && memcmp(buffer, expected, written) == 0);
        return;
    }
    if (strcmp(name, "empty-input") == 0) {
        const uint8_t expected[] = {1, 4, 0, 5, 0, 0, 0, 0};
        CHECK(UmiWebFastCgiInput(UMI_WEB_FASTCGI_PARAMS, 5U, NULL, 0U,
            buffer, sizeof buffer, &written) == UMI_STATUS_OK);
        CHECK(written == sizeof expected && memcmp(buffer, expected, written) == 0);
        CHECK(buffer[written] == 0x7a);
        return;
    }
    if (strcmp(name, "large-input") == 0) {
        const size_t capacity = (size_t)UMI_WEB_FASTCGI_CONTENT_LIMIT + 9U;
        uint8_t *body = malloc(UMI_WEB_FASTCGI_CONTENT_LIMIT);
        uint8_t *wire = malloc(capacity);
        UmiWebFastCgiRecord record;
        CHECK(body != NULL && wire != NULL);
        memset(body, 0xbf, UMI_WEB_FASTCGI_CONTENT_LIMIT);
        CHECK(UmiWebFastCgiInput(UMI_WEB_FASTCGI_STDIN, 65535U, body, UMI_WEB_FASTCGI_CONTENT_LIMIT,
            wire, capacity, &written) == UMI_STATUS_OK);
        CHECK(written == capacity && wire[4] == 255U && wire[5] == 255U && wire[6] == 1U);
        CHECK(wire[capacity - 1U] == 0U);
        CHECK(UmiWebFastCgiParseRecord(wire, written, &record) == UMI_STATUS_OK);
        CHECK(record.requestId == 65535U && record.bodyBytes == UMI_WEB_FASTCGI_CONTENT_LIMIT);
        CHECK(memcmp(body, record.body, record.bodyBytes) == 0 && record.consumed == capacity);
        CHECK(UmiWebFastCgiInput(UMI_WEB_FASTCGI_STDIN, 65535U, body, (size_t)UINT16_MAX + 1U,
            wire, capacity, &written) == UMI_STATUS_CAPACITY_EXCEEDED);
        free(wire);
        free(body);
        return;
    }
    CHECK(strcmp(name, "short-output") == 0);
    CHECK(UmiWebFastCgiBegin(5U, buffer, 15U, &written) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiWebFastCgiBegin(0U, buffer, sizeof buffer, &written) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebFastCgiInput(UMI_WEB_FASTCGI_STDOUT, 5U, NULL, 0U,
        buffer, sizeof buffer, &written) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(written == 91U);
    for (size_t i = 0U; i < sizeof buffer; ++i) CHECK(buffer[i] == 0x7a);
}

static void CheckParameter(const char *name)
{
    uint8_t buffer[512];
    size_t written = 73U;
    memset(buffer, 0x7a, sizeof buffer);
    if (strcmp(name, "short-parameter") == 0) {
        const uint8_t expected[] = {7, 3, 'M', 'E', 'T', 'H', 'O', 'D', '_', 'G', 'E', 'T'};
        CHECK(UmiWebFastCgiParameter("METHOD_", 7U, "GET", 3U,
            buffer, sizeof buffer, &written) == UMI_STATUS_OK);
        CHECK(written == sizeof expected && memcmp(buffer, expected, written) == 0);
        CHECK(UmiWebFastCgiParameter("_A7", 3U, NULL, 0U,
            buffer, sizeof buffer, &written) == UMI_STATUS_OK && written == 5U && buffer[1] == 0U);
        return;
    }
    if (strcmp(name, "long-parameter") == 0) {
        char field[129], value[128];
        const uint8_t fixture[] = {0x80, 0, 0, 129, 0x80, 0, 0, 128};
        memset(field, 'A', sizeof field);
        memset(value, 'b', sizeof value);
        CHECK(UmiWebFastCgiParameter(field, sizeof field, value, sizeof value,
            buffer, sizeof buffer, &written) == UMI_STATUS_OK);
        CHECK(written == 265U && memcmp(buffer, fixture, sizeof fixture) == 0);
        CHECK(memcmp(buffer + 8, field, sizeof field) == 0);
        CHECK(memcmp(buffer + 8 + sizeof field, value, sizeof value) == 0);
        CHECK(UmiWebFastCgiParameter("A", 1U, value, 127U,
            buffer, sizeof buffer, &written) == UMI_STATUS_OK);
        CHECK(written == 130U && buffer[0] == 1U && buffer[1] == 127U);
        return;
    }
    const char badValue[] = {'a', 0, 'b'};
    CHECK(strcmp(name, "bad-parameter") == 0);
    CHECK(UmiWebFastCgiParameter("9A", 2U, NULL, 0U,
        buffer, sizeof buffer, &written) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebFastCgiParameter("A", 1U, badValue, sizeof badValue,
        buffer, sizeof buffer, &written) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebFastCgiParameter("a", 1U, "b", 1U,
        buffer, sizeof buffer, &written) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebFastCgiParameter("A", 1U, "b", 1U,
        buffer, 3U, &written) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(written == 73U);
    for (size_t i = 0U; i < sizeof buffer; ++i) CHECK(buffer[i] == 0x7a);
}

static void CheckFrames(const char *name)
{
    UmiWebFastCgiRecord record = {.type = 80U};
    if (strcmp(name, "truncated-frame") == 0) {
        for (size_t i = 0U; i < sizeof output; ++i)
            CHECK(UmiWebFastCgiParseRecord(output, i, &record) == UMI_STATUS_UNAVAILABLE);
        CHECK(record.type == 80U);
        return;
    }
    if (strcmp(name, "bad-header") == 0) {
        uint8_t bytes[sizeof output];
        memcpy(bytes, output, sizeof bytes);
        bytes[0] = 2U;
        CHECK(UmiWebFastCgiParseRecord(bytes, sizeof bytes, &record) == UMI_STATUS_PARSE_ERROR);
        bytes[0] = 1U;
        bytes[1] = 0U;
        CHECK(UmiWebFastCgiParseRecord(bytes, sizeof bytes, &record) == UMI_STATUS_PARSE_ERROR);
        CHECK(record.type == 80U);
        return;
    }
    CHECK(strcmp(name, "two-records") == 0);
    uint8_t joined[sizeof output + sizeof complete];
    memcpy(joined, output, sizeof output);
    memcpy(joined + sizeof output, complete, sizeof complete);
    CHECK(UmiWebFastCgiParseRecord(joined, sizeof joined, &record) == UMI_STATUS_OK);
    CHECK(record.consumed == sizeof output && record.bodyBytes == 3U && record.body[1] == 0U);
    const size_t offset = record.consumed;
    CHECK(UmiWebFastCgiParseRecord(joined + offset, sizeof joined - offset, &record) == UMI_STATUS_OK);
    CHECK(record.consumed == sizeof complete && record.type == UMI_WEB_FASTCGI_END_REQUEST);
}

static void CheckReply(const char *name)
{
    UmiWebFastCgiReply reply;
    UmiWebFastCgiRecord record = {.type = 80U};
    const size_t limit = strcmp(name, "shared-budget") == 0 ? 3U : 20U;
    CHECK(UmiWebFastCgiReplyInit(5U, limit, &reply) == UMI_STATUS_OK);

    if (strcmp(name, "response") == 0) {
        CHECK(UmiWebFastCgiReadReply(&reply, output, sizeof output, &record) == UMI_STATUS_OK);
        CHECK(UmiWebFastCgiReadReply(&reply, diagnostic, sizeof diagnostic, &record) == UMI_STATUS_OK);
        CHECK(UmiWebFastCgiReadReply(&reply, closeOutput, sizeof closeOutput, &record) == UMI_STATUS_OK);
        CHECK(reply.stdoutClosed && !reply.complete && reply.stdoutBytes == 3U && reply.stderrBytes == 1U);
        CHECK(UmiWebFastCgiReadReply(&reply, complete, 15U, &record) == UMI_STATUS_UNAVAILABLE);
        CHECK(!reply.complete && record.type == UMI_WEB_FASTCGI_STDOUT);
        CHECK(UmiWebFastCgiReadReply(&reply, complete, sizeof complete, &record) == UMI_STATUS_OK);
        CHECK(reply.complete && reply.stderrClosed && reply.applicationStatus == 0U &&
            reply.protocolStatus == UMI_WEB_FASTCGI_REQUEST_COMPLETE);
        CHECK(UmiWebFastCgiReadReply(&reply, complete, sizeof complete, &record) == UMI_STATUS_INVALID_STATE);
        return;
    }
    if (strcmp(name, "wrong-request") == 0) {
        CHECK(UmiWebFastCgiReplyInit(268U, 20U, &reply) == UMI_STATUS_OK);
        CHECK(UmiWebFastCgiReadReply(&reply, output, sizeof output, &record) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(reply.stdoutBytes == 0U && record.type == 80U);
        return;
    }
    if (strcmp(name, "shared-budget") == 0) {
        CHECK(UmiWebFastCgiReadReply(&reply, output, sizeof output, &record) == UMI_STATUS_OK);
        CHECK(UmiWebFastCgiReadReply(&reply, diagnostic, sizeof diagnostic, &record) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(reply.stdoutBytes == 3U && reply.stderrBytes == 0U && record.type == UMI_WEB_FASTCGI_STDOUT);
        return;
    }
    if (strcmp(name, "closed-stream") == 0) {
        CHECK(UmiWebFastCgiReadReply(&reply, closeOutput, sizeof closeOutput, &record) == UMI_STATUS_OK);
        CHECK(UmiWebFastCgiReadReply(&reply, output, sizeof output, &record) == UMI_STATUS_PARSE_ERROR);
        CHECK(reply.stdoutClosed && reply.stdoutBytes == 0U && record.bodyBytes == 0U);
        return;
    }
    uint8_t finish[sizeof complete];
    memcpy(finish, complete, sizeof finish);
    if (strcmp(name, "end-status") == 0) {
        finish[8] = 0xabU;
        finish[11] = 0x71U;
        finish[12] = UMI_WEB_FASTCGI_OVERLOADED;
        CHECK(UmiWebFastCgiReadReply(&reply, finish, sizeof finish, &record) == UMI_STATUS_OK);
        CHECK(reply.complete && reply.protocolStatus == UMI_WEB_FASTCGI_OVERLOADED &&
            reply.applicationStatus == UINT32_C(0xab000071));
        return;
    }
    CHECK(strcmp(name, "bad-end") == 0);
    finish[12] = 4U;
    CHECK(UmiWebFastCgiReadReply(&reply, finish, sizeof finish, &record) == UMI_STATUS_PARSE_ERROR);
    finish[12] = 0U;
    finish[5] = 7U;
    CHECK(UmiWebFastCgiReadReply(&reply, finish, sizeof finish, &record) == UMI_STATUS_PARSE_ERROR);
    CHECK(!reply.complete && record.type == 80U);
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    if (strstr(name, "parameter") != NULL) CheckParameter(name);
    else if (strstr(name, "frame") != NULL || strcmp(name, "bad-header") == 0 ||
        strcmp(name, "two-records") == 0) CheckFrames(name);
    else if (strcmp(name, "begin") == 0 || strstr(name, "input") != NULL ||
        strcmp(name, "short-output") == 0) CheckEncoding(name);
    else CheckReply(name);
    return EXIT_SUCCESS;
}
