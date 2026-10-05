/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_folding_locations.c
 * PURPOSE: Check complete-line ranges, source boundaries and atomic rejection of malformed folding replies.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/location_catalogue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #c);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"lines",
                           "final-line",
                           "trailing-newline",
                           "crlf",
                           "cr",
                           "unicode",
                           "empty-source",
                           "single-line",
                           "nested",
                           "duplicate",
                           "unordered",
                           "characters-ignored",
                           "future-kind",
                           "null",
                           "empty-array",
                           "object",
                           "null-row",
                           "missing-start",
                           "missing-end",
                           "negative",
                           "fraction",
                           "reversed",
                           "outside",
                           "partial-invalid",
                           "start-character-invalid",
                           "end-character-invalid",
                           "kind-invalid",
                           "label-invalid",
                           "wrong-id",
                           "error",
                           "invalid-uri",
                           "invalid-source",
                           "embedded-null",
                           "null-source",
                           "null-output",
                           "cancelled",
                           "owned",
                           "capacity",
                           "capacity-over"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    char uri[128] = "file:///workspace/main.c", source[128] = "head\nbody\nend\ntail";
    const char *ranges = "[{\"startLine\":0,\"endLine\":2}]";
    size_t expected_count = 1U;
    uint64_t first = 0U, end = 3U, column = 0U, id = 7U;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "final-line") == 0)
    {
        ranges = "[{\"startLine\":0,\"endLine\":3}]";
        column = 4U;
    }
    if (strcmp(mode, "trailing-newline") == 0)
        strcpy(source, "head\nbody\nend\n");
    if (strcmp(mode, "crlf") == 0)
        strcpy(source, "head\r\nbody\r\nend\r\ntail");
    if (strcmp(mode, "cr") == 0)
        strcpy(source, "head\rbody\rend\rtail");
    if (strcmp(mode, "unicode") == 0)
    {
        strcpy(source, "head\n\xf0\x9f\x8c\x8d");
        ranges = "[{\"startLine\":0,\"endLine\":1}]";
        end = 1U;
        column = 2U;
    }
    if (strcmp(mode, "empty-source") == 0)
    {
        source[0] = '\0';
        ranges = "[{\"startLine\":0,\"endLine\":0}]";
        end = 0U;
    }
    if (strcmp(mode, "single-line") == 0)
    {
        ranges = "[{\"startLine\":1,\"endLine\":1}]";
        first = 1U;
        end = 2U;
    }
    if (strcmp(mode, "nested") == 0)
    {
        ranges = "[{\"startLine\":0,\"endLine\":2},{\"startLine\":1,\"endLine\":2}]";
        expected_count = 2U;
    }
    if (strcmp(mode, "duplicate") == 0)
    {
        ranges = "[{\"startLine\":0,\"endLine\":2},{\"startLine\":0,\"endLine\":2}]";
        expected_count = 2U;
    }
    if (strcmp(mode, "unordered") == 0)
    {
        ranges = "[{\"startLine\":1,\"endLine\":2},{\"startLine\":0,\"endLine\":2}]";
        expected_count = 2U;
        first = 1U;
    }
    if (strcmp(mode, "characters-ignored") == 0)
        ranges = "[{\"startLine\":0,\"endLine\":2,\"startCharacter\":2147483647,\"endCharacter\":0}]";
    if (strcmp(mode, "future-kind") == 0)
        ranges =
            "[{\"startLine\":0,\"endLine\":2,\"kind\":\"future-category\",\"collapsedText\":\"<plain>\"}]";
    if (strcmp(mode, "null") == 0 || strcmp(mode, "empty-array") == 0)
    {
        ranges = strcmp(mode, "null") == 0 ? "null" : "[]";
        expected_count = 0U;
    }
    const char *bad[] = {"object",
                         "null-row",
                         "missing-start",
                         "missing-end",
                         "negative",
                         "fraction",
                         "reversed",
                         "partial-invalid",
                         "start-character-invalid",
                         "end-character-invalid",
                         "kind-invalid",
                         "label-invalid"};
    const char *invalid[] = {"{}",
                             "[null]",
                             "[{\"endLine\":1}]",
                             "[{\"startLine\":0}]",
                             "[{\"startLine\":-1,\"endLine\":1}]",
                             "[{\"startLine\":0.5,\"endLine\":1}]",
                             "[{\"startLine\":2,\"endLine\":1}]",
                             "[{\"startLine\":0,\"endLine\":2},{}]",
                             "[{\"startLine\":0,\"endLine\":2,\"startCharacter\":null}]",
                             "[{\"startLine\":0,\"endLine\":2,\"endCharacter\":-1}]",
                             "[{\"startLine\":0,\"endLine\":2,\"kind\":1}]",
                             "[{\"startLine\":0,\"endLine\":2,\"collapsedText\":false}]"};
    for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
        if (strcmp(mode, bad[i]) == 0)
        {
            ranges = invalid[i];
            expected = UMI_STATUS_PARSE_ERROR;
        }
    if (strcmp(mode, "outside") == 0)
    {
        ranges = "[{\"startLine\":0,\"endLine\":4}]";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "wrong-id") == 0)
    {
        id = 8U;
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "invalid-uri") == 0)
    {
        strcpy(uri, "relative.c");
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "invalid-source") == 0)
    {
        strcpy(source, "\xc0\x80");
        expected = UMI_STATUS_PARSE_ERROR;
    }
    size_t bytes = strlen(source);
    if (strcmp(mode, "embedded-null") == 0)
    {
        source[1] = '\0';
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "null-source") == 0 || strcmp(mode, "null-output") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    char *json = malloc(180000U);
    CHECK(json != NULL);
    if (strcmp(mode, "capacity") == 0 || strcmp(mode, "capacity-over") == 0)
    {
        size_t count = strcmp(mode, "capacity") == 0 ? 4096U : 4097U;
        strcpy(json, "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[");
        for (size_t i = 0U; i < count; ++i)
            strcat(json, i == 0U ? "{\"startLine\":0,\"endLine\":2}" : ",{\"startLine\":0,\"endLine\":2}");
        strcat(json, "]}");
        expected_count = count;
        if (count > 4096U)
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else
        (void)snprintf(json, 180000U, "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":%s}", ranges);
    if (strcmp(mode, "error") == 0)
    {
        strcpy(json, "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":-32603,\"message\":\"failed\"}}");
        expected = UMI_STATUS_UNAVAILABLE;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    UmiLanguageLocationCatalogue *catalogue = NULL;
    CHECK(UmiLanguageLocationCatalogueReadFoldingResponse(
              json, strlen(json), id, uri, strcmp(mode, "null-source") == 0 ? NULL : source, bytes, cancel,
              strcmp(mode, "null-output") == 0 ? NULL : &catalogue) == expected);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(catalogue != NULL && UmiLanguageLocationCatalogueCount(catalogue) == expected_count);
        if (strcmp(mode, "owned") == 0)
        {
            memset(json, 0, 180000U);
            memset(source, 0, sizeof(source));
            memset(uri, 0, sizeof(uri));
        }
        if (expected_count != 0U)
        {
            UmiLanguageSourceLocation item;
            CHECK(UmiLanguageLocationCatalogueAt(catalogue, 0U, &item) == UMI_STATUS_OK);
            CHECK(strcmp(item.uri, "file:///workspace/main.c") == 0 && !item.is_link && !item.has_origin);
            CHECK(item.selection.start.line == first && item.selection.start.utf16_column == 0U &&
                  item.selection.end.line == end && item.selection.end.utf16_column == column);
            if (strcmp(mode, "unordered") == 0 || strcmp(mode, "nested") == 0)
            {
                CHECK(UmiLanguageLocationCatalogueAt(catalogue, 1U, &item) == UMI_STATUS_OK);
                CHECK(item.selection.start.line == (strcmp(mode, "nested") == 0 ? 1U : 0U));
            }
        }
    }
    else
        CHECK(catalogue == NULL);
    UmiLanguageLocationCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    free(json);
    return 0;
}
