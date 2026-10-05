/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_diagnostic_context.c
 * PURPOSE: Check exact source-range selection and preservation of opaque diagnostic data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/diagnostic_context.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c);                                          \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {
        "selection", "caret-start", "caret-middle",   "caret-end",          "whole",       "empty-set",
        "owned",     "wrong-uri",   "wrong-version",  "unknown-version",    "unversioned", "reversed",
        "outside",   "split-utf8",  "invalid-source", "invalid-diagnostic", "cancelled",   "arguments",
        "raw"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    const char *items =
        "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":2}},"
        "\"message\":\"left\"},{ "
        "\"range\":{\"start\":{\"line\":0,\"character\":2},\"end\":{\"line\":0,\"character\":4}}, "
        "\"message\":\"middle\", \"code\":7, \"data\":{\"opaque\":[1,\"session-token\"]} "
        "},{\"range\":{\"start\":{\"line\":0,\"character\":4},\"end\":{\"line\":0,\"character\":4}},"
        "\"message\":\"point\"},{\"range\":{\"start\":{\"line\":0,\"character\":4},\"end\":{\"line\":0,"
        "\"character\":6}},\"message\":\"right\"}]";
    if (strcmp(mode, "empty-set") == 0)
        items = "[]";
    char json[4096];
    (void)snprintf(json, sizeof(json), "{\"uri\":\"file:///main.c\",%s\"diagnostics\":%s}",
                   strcmp(mode, "unversioned") == 0 ? "" : "\"version\":1,", items);
    UmiLanguageDiagnosticCatalogue *catalogue = NULL;
    CHECK(UmiLanguageDiagnosticCatalogueCreate(json, strlen(json), NULL, &catalogue) == UMI_STATUS_OK);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    const char *source = "abcdef", *uri = "file:///main.c";
    size_t bytes = 6U, start = 2U, end = 4U, count = 2U;
    int32_t version = 1;
    const int32_t *known_version = &version;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "caret-start") == 0)
    {
        start = end = 0U;
        count = 1U;
    }
    if (strcmp(mode, "caret-middle") == 0)
    {
        start = end = 2U;
        count = 2U;
    }
    if (strcmp(mode, "caret-end") == 0)
    {
        start = end = 4U;
        count = 3U;
    }
    if (strcmp(mode, "whole") == 0)
    {
        start = 0U;
        end = 6U;
        count = 4U;
    }
    if (strcmp(mode, "empty-set") == 0)
        count = 0U;
    if (strcmp(mode, "wrong-uri") == 0)
    {
        uri = "file:///other.c";
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "wrong-version") == 0)
    {
        version = 2;
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "unknown-version") == 0)
    {
        known_version = NULL;
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (strcmp(mode, "unversioned") == 0)
        known_version = NULL;
    if (strcmp(mode, "reversed") == 0)
    {
        start = 5U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "outside") == 0)
    {
        end = 7U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "split-utf8") == 0)
    {
        source = "ab\xf0\x9f\x98\x80"
                 "cd";
        bytes = 8U;
        start = 3U;
        end = 6U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-source") == 0)
    {
        source = "abcde\xff";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "invalid-diagnostic") == 0)
    {
        source = "abcd";
        bytes = 4U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    UmiLanguageDiagnosticContext *context = (UmiLanguageDiagnosticContext *)1;
    CHECK(UmiLanguageDiagnosticContextCreate(catalogue, uri, known_version, source, bytes, start, end, cancel,
                                             &context) == expected);
    if (expected == UMI_STATUS_OK)
    {
        if (strcmp(mode, "owned") == 0)
        {
            UmiLanguageDiagnosticCatalogueDestroy(catalogue);
            catalogue = NULL;
            memset(json, 'x', sizeof(json));
        }
        CHECK(UmiLanguageDiagnosticContextCount(context) == count);
        const char *raw = NULL;
        size_t size = 0U;
        CHECK(UmiLanguageDiagnosticContextJson(context, &raw, &size) == UMI_STATUS_OK && raw[0] == '[' &&
              size == strlen(raw) && raw[size - 1U] == ']');
        if (strcmp(mode, "selection") == 0 || strcmp(mode, "raw") == 0 || strcmp(mode, "owned") == 0)
        {
            CHECK(strstr(raw, "\"middle\"") != NULL && strstr(raw, "\"point\"") != NULL);
            CHECK(strstr(raw, "\"left\"") == NULL && strstr(raw, "\"right\"") == NULL);
            CHECK(strstr(raw, " \"code\":7, \"data\":{\"opaque\":[1,\"session-token\"]} ") != NULL);
        }
        if (strcmp(mode, "empty-set") == 0)
            CHECK(strcmp(raw, "[]") == 0);
        if (strcmp(mode, "arguments") == 0)
        {
            CHECK(UmiLanguageDiagnosticContextJson(NULL, &raw, &size) == UMI_STATUS_INVALID_ARGUMENT &&
                  raw == NULL && size == 0U);
            CHECK(UmiLanguageDiagnosticContextCreate(catalogue, uri, known_version, source, bytes, start, end,
                                                     NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLanguageDiagnosticContextCount(NULL) == 0U);
            UmiLanguageDiagnosticContextDestroy(NULL);
        }
    }
    else
        CHECK(context == NULL);
    UmiLanguageDiagnosticContextDestroy(context);
    UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
