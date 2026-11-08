/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_request_stream.c
 *
 * PURPOSE:
 *   Check owned incremental HTTP framing, partition independence, terminal errors and explicit reuse.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/request_stream.h"
#include "../../src/web/request_stream_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
static const char get[] = "GET /hello?name=world HTTP/1.1\r\nHost: localhost\r\n\r\n";
static const char post[] = "POST /submit HTTP/1.1\r\nHost: localhost\r\nContent-Length: 3\r\n\r\nabc";
static const char binary[] = "POST /binary HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5\r\n\r\nA\0\r\nB";
static int Preserved(const UmiWebRequest *request)
{
    for (size_t i = 0U; i < sizeof(*request); ++i)
        if (((const unsigned char *)request)[i] != 0xa5U) return 0;
    return 1;
}
static int ParseResult(UmiWebRequestStream *stream, UmiStatus expected, size_t length)
{
    UmiWebRequest *request = malloc(sizeof(*request));
    CHECK(request != NULL);
    memset(request, 0xa5, sizeof(*request));
    CHECK(UmiWebRequestStreamParse(stream, request) == expected);
    UmiWebRequestStreamSnapshot snapshot;
    CHECK(UmiWebRequestStreamInspect(stream, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.status == expected);
    if (expected == UMI_STATUS_OK) {
        CHECK(snapshot.received_bytes == length && snapshot.frame.message_bytes == length);
        CHECK(snapshot.frame.header_bytes + snapshot.frame.body_bytes == length);
        CHECK(request->body_length == snapshot.frame.body_bytes);
        CHECK(strcmp(umi_web_request_header(request, "Host"), "localhost") == 0);
    } else CHECK(Preserved(request));
    free(request);
    return 0;
}
static int Chunks(const void *bytes, size_t length, size_t step, size_t maximum, UmiStatus expected)
{
    UmiWebRequestStream *stream = NULL;
    CHECK(UmiWebRequestStreamCreate(maximum, &stream) == UMI_STATUS_OK);
    UmiStatus status = UMI_STATUS_BUSY;
    for (size_t position = 0U; position < length;) {
        const size_t count = length - position < step ? length - position : step;
        status = UmiWebRequestStreamFeed(stream, (const unsigned char *)bytes + position, count);
        position += count;
        if (status != UMI_STATUS_BUSY && status != UMI_STATUS_OK) break;
        if (position < length) CHECK(status == UMI_STATUS_BUSY || expected != UMI_STATUS_OK);
    }
    CHECK(status == expected);
    CHECK(ParseResult(stream, expected, length) == 0);
    UmiWebRequestStreamDestroy(stream);
    return 0;
}
/* Different transport partitions must not change the final strict decision.
 * These are independent wire examples, including a second message after a
 * complete first one. Delayed semantic refusal is permitted until headers end. */
static int Partitions(void)
{
    const struct { const char *wire; UmiStatus status; } vectors[] = {
        {get, UMI_STATUS_OK}, {post, UMI_STATUS_OK},
        {"GET / HTTP/1.1\r\n\r\n", UMI_STATUS_PARSE_ERROR},
        {"GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\nContent-Length: 0\r\n\r\n", UMI_STATUS_PARSE_ERROR},
        {"POST / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n", UMI_STATUS_NOT_IMPLEMENTED},
        {"GET / HTTP/1.0\r\nHost: localhost\r\n\r\n", UMI_STATUS_NOT_IMPLEMENTED},
        {"GET / HTTP/1.1\nHost: localhost\n\n", UMI_STATUS_PARSE_ERROR},
        {"GET / HTTP/1.1\rX", UMI_STATUS_PARSE_ERROR},
        {"GET / HTTP/1.1\r\nHost: localhost\r\n\r\nextra", UMI_STATUS_PARSE_ERROR},
        {"GET / HTTP/1.1\r\nHost: localhost\r\n\r\nGET / HTTP/1.1\r\nHost: localhost\r\n\r\n", UMI_STATUS_PARSE_ERROR}
    };
    for (size_t i = 0U; i < sizeof(vectors) / sizeof(vectors[0]); ++i) {
        const size_t length = strlen(vectors[i].wire);
        UmiWebRequestFrame frame;
        CHECK(UmiWebRequestFrameRead(vectors[i].wire, length, 1024U, &frame) == vectors[i].status);
        for (size_t split = 0U; split <= length; ++split) {
            UmiWebRequestStream *stream = NULL;
            CHECK(UmiWebRequestStreamCreate(1024U, &stream) == UMI_STATUS_OK);
            (void)UmiWebRequestStreamFeed(stream, vectors[i].wire, split);
            CHECK(UmiWebRequestStreamFeed(stream, vectors[i].wire + split, length - split) == vectors[i].status);
            CHECK(ParseResult(stream, vectors[i].status, length) == 0);
            UmiWebRequestStreamDestroy(stream);
        }
    }
    return 0;
}
static int ManyFragments(void)
{
    unsigned char *wire = malloc(UMI_WEB_CONNECTION_REQUEST_LIMIT);
    CHECK(wire != NULL);
    const char prefix[] = "POST /large HTTP/1.1\r\nHost: localhost\r\nContent-Length: 8192\r\n";
    size_t used = sizeof(prefix) - 1U;
    memcpy(wire, prefix, used);
    for (unsigned i = 0U; i < 30U; ++i) {
        int wrote = snprintf((char *)wire + used, UMI_WEB_CONNECTION_REQUEST_LIMIT - used, "X-Note-%u: ", i);
        CHECK(wrote > 0 && (size_t)wrote < UMI_WEB_CONNECTION_REQUEST_LIMIT - used);
        used += (size_t)wrote;
        memset(wire + used, 'a', 256U); used += 256U;
        memcpy(wire + used, "\r\n", 2U); used += 2U;
    }
    memcpy(wire + used, "\r\n", 2U); used += 2U;
    const size_t header = used;
    for (size_t i = 0U; i < 8192U; ++i) wire[used++] = (unsigned char)(i % 256U);
    UmiWebRequestStream *stream = NULL;
    CHECK(UmiWebRequestStreamCreate(UMI_WEB_CONNECTION_REQUEST_LIMIT, &stream) == UMI_STATUS_OK);
    for (size_t i = 0U; i < used; ++i)
        CHECK(UmiWebRequestStreamFeed(stream, wire + i, 1U) == (i + 1U == used ? UMI_STATUS_OK : UMI_STATUS_BUSY));
    UmiWebRequest *request = malloc(sizeof(*request));
    CHECK(request != NULL);
    CHECK(UmiWebRequestStreamParse(stream, request) == UMI_STATUS_OK);
    CHECK(request->body_length == 8192U && memcmp(request->body, wire + header, 8192U) == 0);
    CHECK(request->header_count == 32U);
    free(request); UmiWebRequestStreamDestroy(stream); free(wire);
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    if (strcmp(mode, "all-splits") == 0) return Partitions();
    if (strcmp(mode, "many-fragments") == 0) return ManyFragments();
    if (strcmp(mode, "get-single") == 0) return Chunks(get, sizeof(get) - 1U, sizeof(get), 1024U, UMI_STATUS_OK);
    if (strcmp(mode, "get-bytewise") == 0) return Chunks(get, sizeof(get) - 1U, 1U, 1024U, UMI_STATUS_OK);
    if (strcmp(mode, "post-bytewise") == 0) return Chunks(post, sizeof(post) - 1U, 1U, 1024U, UMI_STATUS_OK);
    if (strcmp(mode, "binary-body") == 0) return Chunks(binary, sizeof(binary) - 1U, 1U, 1024U, UMI_STATUS_OK);
    if (strcmp(mode, "exact-capacity") == 0) return Chunks(get, sizeof(get) - 1U, 1U, sizeof(get) - 1U, UMI_STATUS_OK);
    if (strcmp(mode, "create-invalid") == 0) {
        UmiWebRequestStream *stream = (UmiWebRequestStream *)&stream;
        CHECK(UmiWebRequestStreamCreate(0U, &stream) == UMI_STATUS_INVALID_ARGUMENT && stream == NULL);
        CHECK(UmiWebRequestStreamCreate(UMI_WEB_CONNECTION_REQUEST_LIMIT + 1U, &stream) == UMI_STATUS_INVALID_ARGUMENT && stream == NULL);
        CHECK(UmiWebRequestStreamCreate(1024U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        UmiWebRequestStreamDestroy(NULL);
        return 0;
    }
    UmiWebRequestStream *stream = NULL;
    CHECK(UmiWebRequestStreamCreate(1024U, &stream) == UMI_STATUS_OK);
    UmiWebRequestStreamSnapshot snapshot;
    if (strcmp(mode, "initial") == 0) {
        CHECK(UmiWebRequestStreamInspect(stream, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.received_bytes == 0U && snapshot.maximum_bytes == 1024U);
        CHECK(snapshot.frame.header_bytes == 0U && snapshot.frame.message_bytes == 0U);
        CHECK(snapshot.status == UMI_STATUS_BUSY);
    } else if (strcmp(mode, "empty") == 0) {
        CHECK(UmiWebRequestStreamFeed(stream, NULL, 0U) == UMI_STATUS_BUSY);
        CHECK(UmiWebRequestStreamFeed(stream, get, 5U) == UMI_STATUS_BUSY);
        CHECK(UmiWebRequestStreamFeed(stream, NULL, 0U) == UMI_STATUS_BUSY);
        CHECK(UmiWebRequestStreamInspect(stream, &snapshot) == UMI_STATUS_OK && snapshot.received_bytes == 5U);
        CHECK(UmiWebRequestStreamFeed(stream, get + 5U, sizeof(get) - 6U) == UMI_STATUS_OK);
        CHECK(UmiWebRequestStreamFeed(stream, NULL, 0U) == UMI_STATUS_OK);
    } else if (strcmp(mode, "delayed-semantic") == 0) {
        const char line[] = "GET / HTTP/1.0\r\n";
        const char fields[] = "Host: localhost\r\n\r\n";
        UmiWebRequestFrame frame;
        CHECK(UmiWebRequestFrameRead(line, sizeof(line) - 1U, 1024U, &frame) == UMI_STATUS_NOT_IMPLEMENTED);
        CHECK(UmiWebRequestStreamFeed(stream, line, sizeof(line) - 1U) == UMI_STATUS_BUSY);
        CHECK(UmiWebRequestStreamFeed(stream, fields, sizeof(fields) - 1U) == UMI_STATUS_NOT_IMPLEMENTED);
        CHECK(ParseResult(stream, UMI_STATUS_NOT_IMPLEMENTED, 0U) == 0);
    } else if (strcmp(mode, "owned-input") == 0) {
        unsigned char first[8]; memcpy(first, get, sizeof(first));
        CHECK(UmiWebRequestStreamFeed(stream, first, sizeof(first)) == UMI_STATUS_BUSY);
        memset(first, 'X', sizeof(first));
        CHECK(UmiWebRequestStreamFeed(stream, get + sizeof(first), sizeof(get) - 1U - sizeof(first)) == UMI_STATUS_OK);
        CHECK(ParseResult(stream, UMI_STATUS_OK, sizeof(get) - 1U) == 0);
    } else if (strcmp(mode, "capacity") == 0) {
        CHECK(UmiWebRequestStreamFeed(stream, get, SIZE_MAX) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiWebRequestStreamInspect(stream, &snapshot) == UMI_STATUS_OK && snapshot.received_bytes == 0U);
        CHECK(UmiWebRequestStreamFeed(stream, get, sizeof(get) - 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (strcmp(mode, "full-incomplete") == 0) {
        unsigned char bytes[1024]; memset(bytes, 'A', sizeof(bytes));
        CHECK(UmiWebRequestStreamFeed(stream, bytes, sizeof(bytes)) == UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (strcmp(mode, "declared-overlimit") == 0) {
        const char wire[] = "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 1024\r\n\r\n";
        CHECK(UmiWebRequestStreamFeed(stream, wire, sizeof(wire) - 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (strcmp(mode, "pipeline-later") == 0) {
        CHECK(UmiWebRequestStreamFeed(stream, get, sizeof(get) - 1U) == UMI_STATUS_OK);
        CHECK(UmiWebRequestStreamFeed(stream, NULL, 0U) == UMI_STATUS_OK);
        CHECK(UmiWebRequestStreamFeed(stream, get, 1U) == UMI_STATUS_PARSE_ERROR);
        CHECK(ParseResult(stream, UMI_STATUS_PARSE_ERROR, 0U) == 0);
    } else if (strcmp(mode, "reset-complete") == 0 || strcmp(mode, "reset-failure") == 0) {
        if (strcmp(mode, "reset-complete") == 0)
            CHECK(UmiWebRequestStreamFeed(stream, get, sizeof(get) - 1U) == UMI_STATUS_OK);
        else CHECK(UmiWebRequestStreamFeed(stream, "\n", 1U) == UMI_STATUS_PARSE_ERROR);
        CHECK(UmiWebRequestStreamReset(stream) == UMI_STATUS_OK);
        CHECK(UmiWebRequestStreamInspect(stream, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.received_bytes == 0U && snapshot.frame.message_bytes == 0U && snapshot.status == UMI_STATUS_BUSY);
        CHECK(UmiWebRequestStreamFeed(stream, post, sizeof(post) - 1U) == UMI_STATUS_OK);
        CHECK(ParseResult(stream, UMI_STATUS_OK, sizeof(post) - 1U) == 0);
    } else if (strcmp(mode, "null-input-preserves") == 0) {
        CHECK(UmiWebRequestStreamFeed(stream, get, 5U) == UMI_STATUS_BUSY);
        CHECK(UmiWebRequestStreamFeed(stream, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiWebRequestStreamInspect(stream, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.received_bytes == 5U && snapshot.status == UMI_STATUS_BUSY);
        CHECK(UmiWebRequestStreamFeed(stream, get + 5U, sizeof(get) - 6U) == UMI_STATUS_OK);
    } else if (strcmp(mode, "null-output") == 0) {
        CHECK(UmiWebRequestStreamInspect(stream, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiWebRequestStreamParse(stream, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiWebRequestStreamReset(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiWebRequestStreamFeed(NULL, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(mode, "busy-output-preserved") == 0) {
        CHECK(UmiWebRequestStreamFeed(stream, get, 4U) == UMI_STATUS_BUSY);
        CHECK(ParseResult(stream, UMI_STATUS_BUSY, 0U) == 0);
    } else if (strcmp(mode, "error-output-preserved") == 0) {
        CHECK(UmiWebRequestStreamFeed(stream, "\n", 1U) == UMI_STATUS_PARSE_ERROR);
        CHECK(ParseResult(stream, UMI_STATUS_PARSE_ERROR, 0U) == 0);
    } else if (strcmp(mode, "snapshot-invalid") == 0) {
        memset(&snapshot, 0xa5, sizeof(snapshot));
        CHECK(UmiWebRequestStreamInspect(NULL, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        for (size_t i = 0U; i < sizeof(snapshot); ++i) CHECK(((unsigned char *)&snapshot)[i] == 0U);
    } else if (strcmp(mode, "body-pending") == 0) {
        CHECK(UmiWebRequestStreamFeed(stream, post, sizeof(post) - 4U) == UMI_STATUS_BUSY);
        CHECK(UmiWebRequestStreamInspect(stream, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.frame.body_bytes == 3U && snapshot.frame.message_bytes == sizeof(post) - 1U);
        CHECK(ParseResult(stream, UMI_STATUS_BUSY, 0U) == 0);
    } else if (strcmp(mode, "direct-read") == 0) {
        void *bytes = NULL; size_t capacity = 0U;
        CHECK(UmiWebRequestStreamWritable(stream, &bytes, &capacity) == UMI_STATUS_OK && capacity == 1024U);
        memcpy(bytes, get, 5U);
        CHECK(UmiWebRequestStreamCommit(stream, 5U) == UMI_STATUS_BUSY);
        CHECK(UmiWebRequestStreamWritable(stream, &bytes, &capacity) == UMI_STATUS_OK && capacity == 1019U);
        memcpy(bytes, get + 5U, sizeof(get) - 6U);
        CHECK(UmiWebRequestStreamCommit(stream, sizeof(get) - 6U) == UMI_STATUS_OK);
        CHECK(ParseResult(stream, UMI_STATUS_OK, sizeof(get) - 1U) == 0);
        CHECK(UmiWebRequestStreamWritable(stream, &bytes, &capacity) == UMI_STATUS_INVALID_STATE && bytes == NULL && capacity == 0U);
    } else { fprintf(stderr, "Unknown case: %s\n", mode); UmiWebRequestStreamDestroy(stream); return 2; }
    UmiWebRequestStreamDestroy(stream);
    return 0;
}
