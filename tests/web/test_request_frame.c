/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_request_frame.c
 * PURPOSE: Check independent HTTP request examples, ambiguous framing refusals and atomic parsed outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *cases[] = {"get",
                                            "post",
                                            "binary-body",
                                            "head",
                                            "query",
                                            "trimmed",
                                            "length-zero",
                                            "partial-line",
                                            "partial-header",
                                            "partial-body",
                                            "duplicate-length",
                                            "conflicting-length",
                                            "length-list",
                                            "negative-length",
                                            "length-overflow",
                                            "oversized-body",
                                            "missing-host",
                                            "duplicate-host",
                                            "empty-host",
                                            "invalid-host",
                                            "bare-lf",
                                            "bad-cr",
                                            "folded-header",
                                            "whitespace-name",
                                            "control-header",
                                            "null-header",
                                            "unsupported-method",
                                            "unsupported-version",
                                            "chunked",
                                            "expect",
                                            "upgrade",
                                            "pipeline",
                                            "extra-body",
                                            "invalid-target",
                                            "path-capacity",
                                            "limit",
                                            "null-input",
                                            "null-output",
                                            "unchanged"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    const char *wire = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    UmiStatus expected = UMI_STATUS_OK;
    const struct
    {
        const char *name, *wire;
        UmiStatus status;
    } examples[] = {
        {"post", "POST /submit HTTP/1.1\r\nHost: localhost\r\nContent-Length: 3\r\n\r\nabc", UMI_STATUS_OK},
        {"head", "HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n", UMI_STATUS_OK},
        {"query", "GET /hello?name=world HTTP/1.1\r\nHost: localhost\r\n\r\n", UMI_STATUS_OK},
        {"trimmed", "GET / HTTP/1.1\r\nhOsT:\t localhost \t\r\nX-Note: \t hello world \t\r\n\r\n",
         UMI_STATUS_OK},
        {"length-zero", "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n", UMI_STATUS_OK},
        {"partial-line", "GET / HTTP/1.", UMI_STATUS_BUSY},
        {"partial-header", "GET / HTTP/1.1\r\nHost: local", UMI_STATUS_BUSY},
        {"partial-body", "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 4\r\n\r\nab",
         UMI_STATUS_BUSY},
        {"duplicate-length",
         "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\nContent-Length: 0\r\n\r\n",
         UMI_STATUS_PARSE_ERROR},
        {"conflicting-length",
         "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\nContent-Length: 1\r\n\r\n",
         UMI_STATUS_PARSE_ERROR},
        {"length-list", "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0, 0\r\n\r\n",
         UMI_STATUS_PARSE_ERROR},
        {"negative-length", "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: -1\r\n\r\n",
         UMI_STATUS_PARSE_ERROR},
        {"length-overflow",
         "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 9999999999999999999999999999\r\n\r\n",
         UMI_STATUS_CAPACITY_EXCEEDED},
        {"oversized-body", "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 16384\r\n\r\n",
         UMI_STATUS_CAPACITY_EXCEEDED},
        {"missing-host", "GET / HTTP/1.1\r\n\r\n", UMI_STATUS_PARSE_ERROR},
        {"duplicate-host", "GET / HTTP/1.1\r\nHost: one\r\nHost: two\r\n\r\n", UMI_STATUS_PARSE_ERROR},
        {"empty-host", "GET / HTTP/1.1\r\nHost: \r\n\r\n", UMI_STATUS_PARSE_ERROR},
        {"invalid-host", "GET / HTTP/1.1\r\nHost: a/b\r\n\r\n", UMI_STATUS_PARSE_ERROR},
        {"bare-lf", "GET / HTTP/1.1\nHost: localhost\n\n", UMI_STATUS_PARSE_ERROR},
        {"bad-cr", "GET / HTTP/1.1\rx", UMI_STATUS_PARSE_ERROR},
        {"folded-header", "GET / HTTP/1.1\r\nHost: localhost\r\n X-Note: folded\r\n\r\n",
         UMI_STATUS_PARSE_ERROR},
        {"whitespace-name", "GET / HTTP/1.1\r\nHost : localhost\r\n\r\n", UMI_STATUS_PARSE_ERROR},
        {"control-header", "GET / HTTP/1.1\r\nHost: local\x01host\r\n\r\n", UMI_STATUS_PARSE_ERROR},
        {"unsupported-method", "CONNECT / HTTP/1.1\r\nHost: localhost\r\n\r\n", UMI_STATUS_NOT_IMPLEMENTED},
        {"unsupported-version", "GET / HTTP/1.0\r\nHost: localhost\r\n\r\n", UMI_STATUS_NOT_IMPLEMENTED},
        {"chunked", "POST / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n",
         UMI_STATUS_NOT_IMPLEMENTED},
        {"expect", "POST / HTTP/1.1\r\nHost: localhost\r\nExpect: 100-continue\r\n\r\n",
         UMI_STATUS_NOT_IMPLEMENTED},
        {"upgrade", "GET / HTTP/1.1\r\nHost: localhost\r\nUpgrade: websocket\r\n\r\n",
         UMI_STATUS_NOT_IMPLEMENTED},
        {"pipeline", "GET / HTTP/1.1\r\nHost: localhost\r\n\r\nGET / HTTP/1.1\r\nHost: localhost\r\n\r\n",
         UMI_STATUS_PARSE_ERROR},
        {"extra-body", "GET / HTTP/1.1\r\nHost: localhost\r\n\r\nx", UMI_STATUS_PARSE_ERROR},
        {"invalid-target", "GET http://localhost/ HTTP/1.1\r\nHost: localhost\r\n\r\n",
         UMI_STATUS_PARSE_ERROR},
        {"unchanged", "GET / HTTP/1.1\r\n\r\n", UMI_STATUS_PARSE_ERROR}};
    for (size_t i = 0U; i < sizeof(examples) / sizeof(examples[0]); ++i)
        if (strcmp(mode, examples[i].name) == 0)
        {
            wire = examples[i].wire;
            expected = examples[i].status;
        }
    size_t length = strlen(wire), maximum = UMI_WEB_CONNECTION_REQUEST_LIMIT;
    unsigned char altered[2048];
    if (strcmp(mode, "binary-body") == 0)
    {
        wire = "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 3\r\n\r\nA\0B";
        length = sizeof("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 3\r\n\r\nA\0B") - 1U;
    }
    if (strcmp(mode, "null-header") == 0)
    {
        memcpy(altered, wire, length);
        altered[25] = 0U;
        wire = (const char *)altered;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "path-capacity") == 0)
    {
        memcpy(altered, "GET /", 5U);
        memset(altered + 5U, 'a', 600U);
        const char suffix[] = " HTTP/1.1\r\nHost: localhost\r\n\r\n";
        memcpy(altered + 605U, suffix, sizeof(suffix) - 1U);
        wire = (const char *)altered;
        length = 605U + sizeof(suffix) - 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "limit") == 0)
    {
        maximum = length - 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "null-input") == 0)
    {
        wire = NULL;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiWebRequestFrame frame = {1U, 2U, 3U};
    UmiStatus status =
        UmiWebRequestFrameRead(wire, length, maximum, strcmp(mode, "null-output") == 0 ? NULL : &frame);
    if (strcmp(mode, "null-output") == 0)
    {
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    UmiWebRequest *request = malloc(sizeof(*request));
    CHECK(request != NULL);
    memset(request, 0xa5, sizeof(*request));
    UmiStatus parsed = UmiWebRequestParseStrict(wire, length, maximum, request);
    CHECK(parsed == expected);
    CHECK(status == expected);
    if (parsed != UMI_STATUS_OK)
    {
        for (size_t i = 0U; i < sizeof(*request); ++i)
            CHECK(((unsigned char *)request)[i] == 0xa5U);
    }
    else
    {
        CHECK(frame.message_bytes == length && frame.header_bytes + frame.body_bytes == length);
        CHECK(strcmp(umi_web_request_header(request, "Host"), "localhost") == 0);
        if (strcmp(mode, "post") == 0)
            CHECK(request->body_length == 3U && memcmp(request->body, "abc", 3U) == 0);
        if (strcmp(mode, "binary-body") == 0)
            CHECK(request->body_length == 3U && memcmp(request->body, "A\0B", 3U) == 0);
        if (strcmp(mode, "head") == 0)
            CHECK(request->method == UMI_HTTP_METHOD_HEAD);
        if (strcmp(mode, "query") == 0)
            CHECK(strcmp(request->path, "/hello") == 0 && strcmp(request->query, "name=world") == 0);
        if (strcmp(mode, "trimmed") == 0)
            CHECK(strcmp(umi_web_request_header(request, "x-note"), "hello world") == 0);
    }
    free(request);
    return 0;
}
