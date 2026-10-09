/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_sources/test_content.c
 * PURPOSE: Verify source text decoding, UTF-8 fidelity and atomic rejection of oversized or malformed content.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/source_catalog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(v)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(v))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%d: %s\n", __LINE__, #v);                                             \
            failed = 1;                                                                            \
            goto done;                                                                             \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1], *body = NULL;
    UmiStatus expected = UMI_STATUS_OK;
    const char *command = "source";
    int success = 1;
    if (strcmp(mode, "normal") == 0)
        body = "{\"content\":\"int main(void) { return 0; }\\n\",\"mimeType\":\"text/x-c\"}";
    else if (strcmp(mode, "empty") == 0)
        body = "{\"content\":\"\"}";
    else if (strcmp(mode, "unicode") == 0)
        body = "{\"content\":\"caf\\u00e9\\n\\uD83D\\uDE00\"}";
    else if (strcmp(mode, "markup") == 0)
        body = "{\"content\":\"<script>alert(1)</script>\",\"mimeType\":\"text/html\"}";
    else if (strcmp(mode, "missing") == 0)
    {
        body = "{}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "boolean") == 0)
    {
        body = "{\"content\":false}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "nul") == 0)
    {
        body = "{\"content\":\"a\\u0000b\"}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "surrogate") == 0)
    {
        body = "{\"content\":\"\\uD800\"}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "mime") == 0)
    {
        body = "{\"content\":\"\",\"mimeType\":false}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "duplicate") == 0)
    {
        body = "{\"content\":\"a\",\"cont\\u0065nt\":\"b\"}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "command") == 0)
    {
        body = "{\"content\":\"x\"}";
        command = "evaluate";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "refuse") == 0)
    {
        body = "{}";
        success = 0;
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else if (strcmp(mode, "capacity") == 0)
    {
    }
    else
        return 2;
    int failed = 0;
    char *json = malloc(UMI_DEBUG_SOURCE_CONTENT_CAPACITY + 1024U), *large = NULL;
    UmiDebugSourceContent *content = malloc(sizeof *content), *before = malloc(sizeof *before);
    CHECK(json != NULL && content != NULL && before != NULL);
    memset(content, 0xA5, sizeof *content);
    *before = *content;
    if (strcmp(mode, "capacity") == 0)
    {
        large = malloc(UMI_DEBUG_SOURCE_CONTENT_CAPACITY + 32U);
        CHECK(large != NULL);
        strcpy(large, "{\"content\":\"");
        size_t prefix = strlen(large);
        memset(large + prefix, 'x', UMI_DEBUG_SOURCE_CONTENT_CAPACITY);
        strcpy(large + prefix + UMI_DEBUG_SOURCE_CONTENT_CAPACITY, "\"}");
        body = large;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    snprintf(json, UMI_DEBUG_SOURCE_CONTENT_CAPACITY + 1024U,
             "{\"type\":\"response\",\"command\":\"%s\",\"success\":%s,\"body\":%s}", command,
             success ? "true" : "false", body);
    CHECK(UmiDebugSourceContentDecode(json, content) == expected);
    if (expected != UMI_STATUS_OK)
        CHECK(memcmp(content, before, sizeof *content) == 0);
    else
    {
        CHECK(content->bytes == strlen(content->text));
        if (strcmp(mode, "empty") == 0)
            CHECK(content->bytes == 0U && content->mime_type[0] == '\0');
        if (strcmp(mode, "unicode") == 0)
            CHECK(strcmp(content->text, "caf\xc3\xa9\n\xf0\x9f\x98\x80") == 0);
        if (strcmp(mode, "markup") == 0)
            CHECK(strcmp(content->text, "<script>alert(1)</script>") == 0);
    }
done:
    free(json);
    free(large);
    free(content);
    free(before);
    return failed;
}
