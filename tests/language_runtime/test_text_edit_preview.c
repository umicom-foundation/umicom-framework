/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_text_edit_preview.c
 * PURPOSE: Exercise whole-document edit geometry, ownership and reply correlation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/text_edit_preview.h"
#include "umicom/language_runtime/response_tree.h"
#include "umicom/language_runtime/json_writer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition)                                                                                     \
    do                                                                                                       \
    {                                                                                                        \
        if (!(condition))                                                                                    \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition);                                  \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
static void Edit(UmiLanguageRuntimeJsonWriter *writer, unsigned first_line, unsigned first_column,
                 unsigned last_line, unsigned last_column, const char *text, int comma)
{
    if (comma)
        umi_language_runtime_json_writer_raw(writer, ",");
    char range[256];
    int n = snprintf(range, sizeof(range),
                     "{\"range\":{\"start\":{\"line\":%u,\"character\":%u},\"end\":{\"line\":%u,"
                     "\"character\":%u}},\"newText\":",
                     first_line, first_column, last_line, last_column);
    CHECK(n > 0 && (size_t)n < sizeof(range));
    umi_language_runtime_json_writer_raw(writer, range);
    umi_language_runtime_json_writer_string(writer, text);
    umi_language_runtime_json_writer_raw(writer, "}");
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1], *source = "abc", *expected = "aBc";
    size_t source_bytes = 3U, caret = 1U, expected_caret = 2U, expected_count = 1U;
    char json[32768];
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, json, sizeof(json));
    umi_language_runtime_json_writer_raw(&writer, "[");
    int invalid = 0, envelope = 0;
    char *owned_source = NULL, *owned_json = NULL;
    UmiCancellationToken *cancel = NULL;
    if (strcmp(mode, "replace") == 0 || strcmp(mode, "ownership") == 0 || strcmp(mode, "response") == 0)
        Edit(&writer, 0U, 1U, 0U, 2U, "B", 0);
    else if (strcmp(mode, "delete") == 0)
    {
        Edit(&writer, 0U, 1U, 0U, 2U, "", 0);
        expected = "ac";
        expected_caret = 1U;
    }
    else if (strcmp(mode, "insert") == 0)
    {
        Edit(&writer, 0U, 1U, 0U, 1U, "XY", 0);
        expected = "aXYbc";
        expected_caret = 3U;
    }
    else if (strcmp(mode, "empty-source") == 0)
    {
        source = "";
        source_bytes = caret = 0U;
        Edit(&writer, 0U, 0U, 0U, 0U, "hello", 0);
        expected = "hello";
        expected_caret = 5U;
    }
    else if (strcmp(mode, "unsorted") == 0)
    {
        Edit(&writer, 0U, 2U, 0U, 3U, "C", 0);
        Edit(&writer, 0U, 0U, 0U, 1U, "A", 1);
        expected = "AbC";
        caret = 2U;
        expected_caret = 3U;
        expected_count = 2U;
    }
    else if (strcmp(mode, "same-position") == 0 || strcmp(mode, "insert-replace") == 0)
    {
        Edit(&writer, 0U, 1U, 0U, 1U, "X", 0);
        Edit(&writer, 0U, 1U, 0U, 1U, "Y", 1);
        expected = "aXYbc";
        expected_caret = 3U;
        expected_count = 2U;
        if (strcmp(mode, "insert-replace") == 0)
        {
            Edit(&writer, 0U, 1U, 0U, 2U, "B", 1);
            expected = "aXYBc";
            expected_caret = 4U;
            expected_count = 3U;
        }
    }
    else if (strcmp(mode, "overlap") == 0 || strcmp(mode, "insert-after-replace") == 0)
    {
        Edit(&writer, 0U, 1U, 0U, 3U, "X", 0);
        Edit(&writer, 0U, 1U, 0U, strcmp(mode, "overlap") == 0 ? 2U : 1U, "Y", 1);
        invalid = 1;
    }
    else if (strcmp(mode, "adjacent") == 0)
    {
        Edit(&writer, 0U, 0U, 0U, 1U, "AA", 0);
        Edit(&writer, 0U, 1U, 0U, 2U, "BB", 1);
        expected = "AABBc";
        expected_caret = 4U;
        expected_count = 2U;
    }
    else if (strcmp(mode, "caret-before") == 0 || strcmp(mode, "caret-after") == 0)
    {
        Edit(&writer, 0U, 1U, 0U, 2U, "LONG", 0);
        expected = "aLONGc";
        caret = strcmp(mode, "caret-before") == 0 ? 0U : 3U;
        expected_caret = caret == 0U ? 0U : 6U;
    }
    else if (strcmp(mode, "unicode") == 0 || strcmp(mode, "surrogate") == 0)
    {
        source = "a\xf0\x9f\x8c\x8d"
                 "c";
        source_bytes = 6U;
        caret = 5U;
        Edit(&writer, 0U, 1U, 0U, strcmp(mode, "surrogate") == 0 ? 2U : 3U, "B", 0);
        if (strcmp(mode, "surrogate") == 0)
            invalid = 1;
    }
    else if (strcmp(mode, "crlf") == 0)
    {
        source = "a\r\nb\r\n";
        source_bytes = 6U;
        caret = 3U;
        Edit(&writer, 1U, 0U, 1U, 1U, "BB", 0);
        expected = "a\r\nBB\r\n";
        expected_caret = 5U;
    }
    else if (strcmp(mode, "crlf-caret") == 0)
    {
        source = "\n";
        source_bytes = 1U;
        caret = 0U;
        Edit(&writer, 0U, 0U, 0U, 0U, "\r", 0);
        invalid = 1;
    }
    else if (strcmp(mode, "reversed") == 0 || strcmp(mode, "line") == 0 || strcmp(mode, "column") == 0)
    {
        if (strcmp(mode, "reversed") == 0)
            Edit(&writer, 0U, 2U, 0U, 1U, "", 0);
        if (strcmp(mode, "line") == 0)
            Edit(&writer, 9U, 0U, 9U, 0U, "", 0);
        if (strcmp(mode, "column") == 0)
            Edit(&writer, 0U, 9U, 0U, 9U, "", 0);
        invalid = 1;
    }
    else if (strcmp(mode, "empty") == 0 || strcmp(mode, "null") == 0)
    {
        expected = "abc";
        expected_caret = caret;
        expected_count = 0U;
    }
    else if (strcmp(mode, "invalid-source") == 0 || strcmp(mode, "source-nul") == 0 ||
             strcmp(mode, "caret-byte") == 0)
    {
        source = strcmp(mode, "invalid-source") == 0 ? "a\xc0\x80"
                 : strcmp(mode, "source-nul") == 0   ? "a\0b"
                                                     : "a\xc3\xa9";
        source_bytes = 3U;
        caret = strcmp(mode, "caret-byte") == 0 ? 2U : 0U;
        invalid = 1;
    }
    else if (strcmp(mode, "annotation") == 0 || strcmp(mode, "duplicate") == 0 ||
             strcmp(mode, "missing") == 0 || strcmp(mode, "type") == 0 || strcmp(mode, "text-nul") == 0)
    {
        const char *edit = strcmp(mode, "annotation") == 0
                               ? "{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                                 "\"character\":0}},\"newText\":\"x\",\"annotationId\":\"approval\"}"
                           : strcmp(mode, "duplicate") == 0 ? "{\"range\":{},\"range\":{}}"
                           : strcmp(mode, "missing") == 0   ? "{\"range\":{}}"
                           : strcmp(mode, "type") == 0
                               ? "7"
                               : "{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                                 "\"character\":0}},\"newText\":\"\\u0000\"}";
        umi_language_runtime_json_writer_raw(&writer, edit);
        invalid = 1;
    }
    else if (strcmp(mode, "edit-limit") == 0)
    {
        for (unsigned i = 0U; i < 257U; ++i)
            Edit(&writer, 0U, 0U, 0U, 0U, "", i != 0U);
        invalid = 1;
    }
    else if (strcmp(mode, "result-limit") == 0 || strcmp(mode, "source-limit") == 0)
    {
        source_bytes = 16U * 1024U * 1024U + (strcmp(mode, "source-limit") == 0 ? 1U : 0U);
        owned_source = malloc(source_bytes + 1U);
        CHECK(owned_source != NULL);
        memset(owned_source, 'a', source_bytes);
        owned_source[source_bytes] = '\0';
        source = owned_source;
        Edit(&writer, 0U, 0U, 0U, 0U, "X", 0);
        invalid = 1;
    }
    else if (strcmp(mode, "cancelled") == 0)
    {
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        invalid = 1;
    }
    else if (strcmp(mode, "wrong-id") == 0 || strcmp(mode, "error") == 0 || strcmp(mode, "mixed") == 0 ||
             strcmp(mode, "malformed-error") == 0 || strcmp(mode, "response-duplicate") == 0)
    {
        envelope = 1;
        invalid = 1;
    }
    else
        return 2;
    umi_language_runtime_json_writer_raw(&writer, "]");
    CHECK(writer.status == UMI_STATUS_OK);
    if (strcmp(mode, "null") == 0)
        strcpy(json, "null");
    if (envelope)
    {
        const char *reply =
            strcmp(mode, "wrong-id") == 0 ? "{\"jsonrpc\":\"2.0\",\"id\":8,\"result\":[]}"
            : strcmp(mode, "error") == 0
                ? "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":-1,\"message\":\"unavailable\"}}"
            : strcmp(mode, "mixed") == 0
                ? "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[],\"error\":{\"code\":-1,\"message\":\"bad\"}}"
            : strcmp(mode, "malformed-error") == 0
                ? "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":\"bad\",\"message\":\"bad\"}}"
                : "{\"jsonrpc\":\"2.0\",\"id\":7,\"id\":7,\"result\":[]}";
        strcpy(json, reply);
    }
    else if (strcmp(mode, "response") == 0)
    {
        owned_json = malloc(strlen(json) + 64U);
        CHECK(owned_json != NULL);
        (void)sprintf(owned_json, "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":%s}", json);
        envelope = 1;
    }
    if (strcmp(mode, "ownership") == 0)
    {
        owned_source = malloc(4U);
        CHECK(owned_source != NULL);
        memcpy(owned_source, source, 4U);
        source = owned_source;
    }
    UmiLanguageTextEditPreview *preview = NULL;
    UmiStatus status =
        envelope ? UmiLanguageTextEditPreviewReadResponse(owned_json == NULL ? json : owned_json,
                                                          strlen(owned_json == NULL ? json : owned_json), 7U,
                                                          source, source_bytes, caret, cancel, &preview)
                 : UmiLanguageTextEditPreviewCreate(json, strlen(json), source, source_bytes, caret, cancel,
                                                    &preview);
    if (invalid)
        CHECK(status != UMI_STATUS_OK && preview == NULL);
    else
    {
        CHECK(status == UMI_STATUS_OK && preview != NULL);
        if (strcmp(mode, "ownership") == 0)
        {
            memset(owned_source, 'z', 3U);
            memset(json, 'z', strlen(json));
        }
        const char *proposed = NULL;
        size_t bytes = 0U, mapped = 0U;
        CHECK(UmiLanguageTextEditPreviewRead(preview, &proposed, &bytes, &mapped) == UMI_STATUS_OK);
        CHECK(bytes == strlen(expected) && memcmp(proposed, expected, bytes) == 0 &&
              mapped == expected_caret);
        CHECK(UmiLanguageTextEditPreviewCount(preview) == expected_count);
    }
    UmiLanguageTextEditPreviewDestroy(preview);
    umi_cancellation_token_destroy(cancel);
    free(owned_source);
    free(owned_json);
    return 0;
}
