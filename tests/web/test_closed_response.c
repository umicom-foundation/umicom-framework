/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_closed_response.c
 * PURPOSE: Verify unambiguous HTTP response lengths, HEAD behavior and refusal without partial output.
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
                                            "head",
                                            "no-content",
                                            "not-modified",
                                            "binary-body",
                                            "tiny-output",
                                            "status",
                                            "header-count",
                                            "header-name",
                                            "header-control",
                                            "header-unterminated",
                                            "length-header",
                                            "transfer-header",
                                            "connection-header",
                                            "body-limit",
                                            "empty-body"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    UmiWebResponse *response = calloc(1U, sizeof(*response));
    CHECK(response != NULL);
    CHECK(umi_web_response_set_text(response, 200, "text/plain", "hello") == UMI_STATUS_OK);
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "no-content") == 0)
        response->status = 204;
    if (strcmp(mode, "not-modified") == 0)
        response->status = 304;
    if (strcmp(mode, "binary-body") == 0)
    {
        memcpy(response->body, "A\0B", 3U);
        response->body_length = 3U;
    }
    if (strcmp(mode, "empty-body") == 0)
        response->body_length = 0U;
    if (strcmp(mode, "status") == 0)
    {
        response->status = 100;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "header-count") == 0)
    {
        response->header_count = UMI_WEB_MAX_HEADERS + 1U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "header-name") == 0)
    {
        strcpy(response->headers[0].name, "Bad Name");
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "header-control") == 0)
    {
        strcpy(response->headers[0].value, "text/plain\r\nInjected: yes");
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "header-unterminated") == 0)
    {
        memset(response->headers[0].value, 'x', sizeof(response->headers[0].value));
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "length-header") == 0 || strcmp(mode, "transfer-header") == 0 ||
        strcmp(mode, "connection-header") == 0)
    {
        strcpy(response->headers[0].name, strcmp(mode, "length-header") == 0     ? "Content-Length"
                                          : strcmp(mode, "transfer-header") == 0 ? "tRaNsFeR-EnCoDiNg"
                                                                                 : "Connection");
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "body-limit") == 0)
    {
        response->body_length = UMI_WEB_BODY_CAPACITY;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    unsigned char wire[1024];
    memset(wire, 0xa5, sizeof(wire));
    size_t length = 999U, capacity = sizeof(wire) - 1U;
    if (strcmp(mode, "tiny-output") == 0)
    {
        capacity = 5U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    CHECK(UmiWebResponseFormatClosed(response, strcmp(mode, "head") == 0, wire, capacity, &length) ==
          expected);
    if (expected != UMI_STATUS_OK)
    {
        CHECK(length == 0U);
        for (size_t i = 0U; i < sizeof(wire); ++i)
            CHECK(wire[i] == 0xa5U);
    }
    else
    {
        CHECK(length < sizeof(wire));
        wire[length] = 0U;
        char *body = strstr((char *)wire, "\r\n\r\n");
        CHECK(body != NULL);
        body += 4U;
        CHECK(strstr((char *)wire, "Connection: close\r\n") != NULL);
        size_t emitted = length - (size_t)(body - (char *)wire);
        if (strcmp(mode, "no-content") == 0 || strcmp(mode, "not-modified") == 0)
            CHECK(emitted == 0U && strstr((char *)wire, "Content-Length:") == NULL);
        else if (strcmp(mode, "head") == 0)
            CHECK(emitted == 0U && strstr((char *)wire, "Content-Length: 5\r\n") != NULL);
        else if (strcmp(mode, "binary-body") == 0)
            CHECK(emitted == 3U && memcmp(body, "A\0B", 3U) == 0);
        else if (strcmp(mode, "empty-body") == 0)
            CHECK(emitted == 0U && strstr((char *)wire, "Content-Length: 0\r\n") != NULL);
        else
            CHECK(emitted == 5U && memcmp(body, "hello", 5U) == 0);
    }
    free(response);
    return 0;
}
