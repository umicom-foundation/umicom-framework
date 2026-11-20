/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/fastcgi/test_exchange.c
 *
 * PURPOSE:
 *   Simulate a connected worker with deterministic short transfers and
 *   confirm that only a complete successful reply publishes stdout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/fastcgi_exchange.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); exit(EXIT_FAILURE); \
} } while (0)

static const uint8_t workerReply[] = {
    1, 6, 0, 5, 0, 3, 0, 0, 'a', 0, 'b',
    1, 7, 0, 5, 0, 1, 0, 0, '!',
    1, 3, 0, 5, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const uint8_t paddedReply[264] = {1, 6, 0, 5, 0, 1, 255, 0, '*'};

typedef struct Fixture {
    uint8_t *requestBytes;
    size_t writeCapacity;
    size_t written;
    uint8_t reply[sizeof(workerReply) + 1U];
    size_t replySize;
    size_t received;
    size_t fragment;
    int repeatPadded;
    int partialWrite;
    int invalidCount;
    UmiCancellationToken *cancel;
} Fixture;

static UmiStatus WriteWorker(void *context, const void *data, size_t length, size_t *out)
{
    Fixture *fixture = context;
    if (fixture->partialWrite && fixture->written != 0U) return UMI_STATUS_TIMEOUT;
    if (fixture->invalidCount) {
        *out = 0U;
        return UMI_STATUS_OK;
    }
    const size_t count = length < fixture->fragment ? length : fixture->fragment;
    CHECK(count <= fixture->writeCapacity - fixture->written);
    memcpy(fixture->requestBytes + fixture->written, data, count);
    fixture->written += count;
    *out = count;
    umi_cancellation_token_request(fixture->cancel);
    return UMI_STATUS_OK;
}

static UmiStatus ReadWorker(void *context, void *data, size_t capacity, size_t *out)
{
    Fixture *fixture = context;
    if (fixture->repeatPadded) {
        uint8_t *destination = data;
        for (size_t i = 0U; i < capacity; ++i)
            destination[i] = paddedReply[(fixture->received + i) % sizeof paddedReply];
        fixture->received += capacity;
        *out = capacity;
        return UMI_STATUS_OK;
    }
    if (fixture->received == fixture->replySize) return UMI_STATUS_UNAVAILABLE;
    const size_t available = fixture->replySize - fixture->received;
    size_t count = capacity < available ? capacity : available;
    if (count > fixture->fragment) count = fixture->fragment;
    memcpy(data, fixture->reply + fixture->received, count);
    fixture->received += count;
    *out = count;
    return UMI_STATUS_OK;
}

static void CheckRequest(const Fixture *fixture, const UmiWebFastCgiRequest *request)
{
    const uint8_t order[] = {1U, 4U, 4U, 5U, 5U, 5U};
    const uint8_t parameter[] = {14, 3, 'R', 'E', 'Q', 'U', 'E', 'S', 'T',
        '_', 'M', 'E', 'T', 'H', 'O', 'D', 'G', 'E', 'T'};
    const size_t chunk = UMI_WEB_FASTCGI_CONTENT_LIMIT;
    const size_t expectedSizes[] = {8U, sizeof parameter, 0U,
        request->bodyBytes < chunk ? request->bodyBytes : chunk,
        request->bodyBytes < chunk ? 0U : request->bodyBytes - chunk, 0U};
    const size_t records = request->bodyBytes > chunk ? 6U : 5U;
    size_t offset = 0U;
    for (size_t i = 0U; i < records; ++i) {
        UmiWebFastCgiRecord record;
        CHECK(UmiWebFastCgiParseRecord(fixture->requestBytes + offset,
            fixture->written - offset, &record) == UMI_STATUS_OK);
        CHECK(record.requestId == 5U && record.type == order[i] && record.bodyBytes == expectedSizes[i]);
        if (i == 1U) CHECK(memcmp(record.body, parameter, sizeof parameter) == 0);
        if (i == 3U) CHECK(memcmp(record.body, request->body, record.bodyBytes) == 0);
        if (i == 4U && record.bodyBytes != 0U)
            CHECK(memcmp(record.body, (const uint8_t *)request->body + chunk, record.bodyBytes) == 0);
        offset += record.consumed;
    }
    CHECK(offset == fixture->written);
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    const uint8_t shortBody[] = {0x11U, 0x00U, 0xa1U};
    UmiWebFastCgiVariable variables[2] = {{"REQUEST_METHOD", 14U, "GET", 3U},
        {"REQUEST_METHOD", 14U, "POST", 4U}};
    UmiWebFastCgiRequest request = {.requestId = 5U, .variables = variables,
        .variableCount = 1U, .body = shortBody, .bodyBytes = sizeof shortBody, .replyLimit = 16U};
    Fixture fixture = {.writeCapacity = 96U, .replySize = sizeof workerReply, .fragment = 3U};
    memcpy(fixture.reply, workerReply, sizeof workerReply);
    UmiCancellationToken *cancel = NULL;
    uint8_t *largeBody = NULL;
    UmiStatus expected = UMI_STATUS_OK;

    if (strcmp(name, "coalesced") == 0) {
        fixture.fragment = sizeof fixture.reply;
    } else if (strcmp(name, "binary-request") == 0) {
        request.bodyBytes = (size_t)UMI_WEB_FASTCGI_CONTENT_LIMIT + 1U;
        fixture.writeCapacity = request.bodyBytes + 128U;
        fixture.fragment = 4096U;
        largeBody = malloc(request.bodyBytes);
        CHECK(largeBody != NULL);
        memset(largeBody, 0x7f, request.bodyBytes);
        largeBody[1] = 0U;
        largeBody[request.bodyBytes - 1U] = 0xa5U;
        request.body = largeBody;
    } else if (strcmp(name, "bad-variable") == 0) {
        variables[1].name = "bad";
        variables[1].nameBytes = 3U;
        request.variableCount = 2U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "duplicate-variable") == 0) {
        request.variableCount = 2U;
        expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (strcmp(name, "outbound-budget") == 0) {
        request.bodyBytes = UMI_WEB_FASTCGI_EXCHANGE_LIMIT;
        largeBody = calloc(request.bodyBytes, 1U);
        CHECK(largeBody != NULL);
        request.body = largeBody;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "complete-error") == 0) {
        fixture.reply[sizeof workerReply - 5U] = 42U;
    } else if (strcmp(name, "worker-refusal") == 0) {
        fixture.reply[sizeof workerReply - 4U] = UMI_WEB_FASTCGI_OVERLOADED;
    } else if (strcmp(name, "premature-eof") == 0) {
        --fixture.replySize;
        expected = UMI_STATUS_UNAVAILABLE;
    } else if (strcmp(name, "partial-write") == 0) {
        fixture.partialWrite = 1;
        expected = UMI_STATUS_TIMEOUT;
    } else if (strcmp(name, "invalid-count") == 0) {
        fixture.invalidCount = 1;
        expected = UMI_STATUS_IO_ERROR;
    } else if (strcmp(name, "cancellation") == 0) {
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        fixture.cancel = cancel;
        expected = UMI_STATUS_CANCELLED;
    } else if (strcmp(name, "receive-budget") == 0) {
        request.replyLimit = 3U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "wire-budget") == 0) {
        fixture.repeatPadded = 1;
        request.replyLimit = UMI_WEB_FASTCGI_EXCHANGE_LIMIT;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "trailing-frame") == 0) {
        fixture.replySize = sizeof fixture.reply;
        fixture.fragment = sizeof fixture.reply;
        expected = UMI_STATUS_PARSE_ERROR;
    } else {
        CHECK(strcmp(name, "fragmented") == 0);
    }

    fixture.requestBytes = malloc(fixture.writeCapacity);
    CHECK(fixture.requestBytes != NULL);
    const UmiWebConnectionIo transport = {&fixture, ReadWorker, WriteWorker};
    UmiWebFastCgiResult result = {0};
    CHECK(UmiWebFastCgiExchange(&transport, &request, cancel, &result) == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(result.body == NULL && result.bodyBytes == 0U && !result.reply.complete);
        if (expected == UMI_STATUS_INVALID_ARGUMENT || expected == UMI_STATUS_ALREADY_EXISTS ||
            strcmp(name, "outbound-budget") == 0) CHECK(fixture.written == 0U);
        if (expected == UMI_STATUS_TIMEOUT || expected == UMI_STATUS_CANCELLED)
            CHECK(fixture.written == 3U && fixture.received == 0U);
        if (fixture.repeatPadded) CHECK(fixture.received == UMI_WEB_FASTCGI_REPLY_WIRE_LIMIT);
    } else {
        CheckRequest(&fixture, &request);
        CHECK(result.reply.complete && result.reply.stdoutBytes == 3U && result.reply.stderrBytes == 1U);
        if (strcmp(name, "complete-error") == 0) {
            CHECK(result.reply.applicationStatus == 42U && result.body == NULL);
        } else if (strcmp(name, "worker-refusal") == 0) {
            CHECK(result.reply.protocolStatus == UMI_WEB_FASTCGI_OVERLOADED && result.body == NULL);
        } else {
            CHECK(result.bodyBytes == 3U && memcmp(result.body, "a\0b", 3U) == 0);
        }
    }
    UmiWebFastCgiResultDestroy(&result);
    CHECK(result.body == NULL && result.bodyBytes == 0U);
    umi_cancellation_token_destroy(cancel);
    free(fixture.requestBytes);
    free(largeBody);
    return EXIT_SUCCESS;
}
