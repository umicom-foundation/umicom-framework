/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_hover_document.c
 * PURPOSE: Exercise owned hover shapes, literal markup, correlation and legacy output bounds.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/hover_document.h"
#include "umicom/language_runtime/decoders/hover.h"
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
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *json = "{\"contents\":\"hello\"}", *expected = "hello";
    UmiStatus wanted = UMI_STATUS_OK;
    size_t count = 1U;
    UmiLanguageHoverContentKind kind = UMI_LANGUAGE_HOVER_MARKDOWN;
    const char *language = "";
    int envelope = 0, has_range = 0, legacy = 0;
    char *owned = NULL;
    if (strcmp(mode, "string") == 0)
    {
    }
    else if (strcmp(mode, "plain") == 0)
    {
        json = "{\"contents\":{\"kind\":\"plaintext\",\"value\":\"hello\"}}";
        kind = UMI_LANGUAGE_HOVER_PLAIN_TEXT;
    }
    else if (strcmp(mode, "markdown") == 0)
    {
        json = "{\"contents\":{\"kind\":\"markdown\",\"value\":\"**hello**\"}}";
        expected = "**hello**";
    }
    else if (strcmp(mode, "literal-html") == 0)
    {
        json = "{\"contents\":\"<script>alert(1)</script>\"}";
        expected = "<script>alert(1)</script>";
    }
    else if (strcmp(mode, "code") == 0)
    {
        json = "{\"contents\":{\"language\":\"c\",\"value\":\"int value;\"}}";
        kind = UMI_LANGUAGE_HOVER_CODE;
        language = "c";
        expected = "int value;";
    }
    else if (strcmp(mode, "array") == 0 || strcmp(mode, "legacy-array") == 0)
    {
        json = "{\"contents\":[\"intro\",{\"language\":\"c\",\"value\":\"int value;\"},\"details\"]}";
        expected = "intro\n\nint value;\n\ndetails";
        count = 3U;
        legacy = strcmp(mode, "legacy-array") == 0;
    }
    else if (strcmp(mode, "empty-array") == 0)
    {
        json = "{\"contents\":[]}";
        count = 0U;
        expected = "";
    }
    else if (strcmp(mode, "empty-text") == 0)
    {
        json = "{\"contents\":\"\"}";
        expected = "";
    }
    else if (strcmp(mode, "null") == 0)
    {
        json = "null";
        count = 0U;
        expected = "";
    }
    else if (strcmp(mode, "unicode") == 0)
    {
        json = "{\"contents\":\"caf\\u00e9 \\ud83c\\udf0d\"}";
        expected = "caf\xc3\xa9 \xf0\x9f\x8c\x8d";
    }
    else if (strcmp(mode, "range") == 0 || strcmp(mode, "legacy-range") == 0)
    {
        json = "{\"contents\":\"hello\",\"range\":{\"start\":{\"line\":1,\"character\":2},\"end\":{\"line\":"
               "3,\"character\":4}}}";
        has_range = 1;
        legacy = strcmp(mode, "legacy-range") == 0;
    }
    else if (strcmp(mode, "reversed") == 0)
    {
        json = "{\"contents\":\"hello\",\"range\":{\"start\":{\"line\":1,\"character\":2},\"end\":{\"line\":"
               "1,\"character\":1}}}";
        wanted = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "negative") == 0)
    {
        json = "{\"contents\":\"hello\",\"range\":{\"start\":{\"line\":-1,\"character\":0},\"end\":{\"line\":"
               "1,\"character\":1}}}";
        wanted = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "null-range") == 0)
    {
        json = "{\"contents\":\"hello\",\"range\":null}";
        wanted = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "missing") == 0)
    {
        json = "{}";
        wanted = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "duplicate") == 0)
    {
        json = "{\"contents\":\"one\",\"contents\":\"two\"}";
        wanted = UMI_STATUS_ALREADY_EXISTS;
    }
    else if (strcmp(mode, "kind") == 0)
    {
        json = "{\"contents\":{\"kind\":\"html\",\"value\":\"hello\"}}";
        wanted = UMI_STATUS_NOT_IMPLEMENTED;
    }
    else if (strcmp(mode, "ambiguous") == 0)
    {
        json = "{\"contents\":{\"kind\":\"markdown\",\"language\":\"c\",\"value\":\"hello\"}}";
        wanted = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "nested-array") == 0)
    {
        json = "{\"contents\":[[\"hello\"]]}";
        wanted = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "markup-array") == 0)
    {
        json = "{\"contents\":[{\"kind\":\"plaintext\",\"value\":\"hello\"}]}";
        wanted = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "wrong-id") == 0)
    {
        json = "{\"jsonrpc\":\"2.0\",\"id\":9,\"result\":null}";
        envelope = 1;
        wanted = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(mode, "error") == 0)
    {
        json = "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":-1,\"message\":\"refused\"}}";
        envelope = 1;
        wanted = UMI_STATUS_UNAVAILABLE;
    }
    else if (strcmp(mode, "response") == 0)
    {
        json = "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"contents\":\"hello\"}}";
        envelope = 1;
    }
    else if (strcmp(mode, "ownership") == 0)
    {
        owned = malloc(strlen(json) + 1U);
        CHECK(owned != NULL);
        strcpy(owned, json);
        json = owned;
    }
    else if (strcmp(mode, "block-limit") == 0)
    {
        owned = calloc(4096U, 1U);
        CHECK(owned != NULL);
        strcpy(owned, "{\"contents\":[");
        for (size_t i = 0U; i < 257U; ++i)
            strcat(owned, i == 0U ? "\"x\"" : ",\"x\"");
        strcat(owned, "]}");
        json = owned;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(mode, "legacy-capacity") == 0)
    {
        owned = malloc(5000U);
        CHECK(owned != NULL);
        strcpy(owned, "{\"contents\":\"");
        size_t begin = strlen(owned);
        memset(owned + begin, 'x', 4096U);
        strcpy(owned + begin + 4096U, "\"}");
        json = owned;
        legacy = 1;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(mode, "language-limit") == 0)
    {
        owned = malloc(256U);
        CHECK(owned != NULL);
        strcpy(owned, "{\"contents\":{\"language\":\"");
        size_t begin = strlen(owned);
        memset(owned + begin, 'x', 128U);
        strcpy(owned + begin + 128U, "\",\"value\":\"hello\"}}");
        json = owned;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else
        CHECK(strcmp(mode, "cancelled") == 0);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        wanted = UMI_STATUS_CANCELLED;
    }
    UmiLanguageHoverDocument *document = NULL;
    if (legacy)
    {
        char *reply = malloc(strlen(json) + 64U);
        CHECK(reply != NULL);
        (void)sprintf(reply, "{\"result\":%s}", json);
        UmiLanguageRuntimeHoverResult result;
        CHECK(umi_language_runtime_decode_hover(reply, &result) == wanted);
        free(reply);
        if (wanted == UMI_STATUS_OK)
        {
            CHECK(strcmp(result.contents, expected) == 0 && result.has_range == has_range);
            if (has_range)
                CHECK(result.range.start.line == 1U && result.range.end.character == 4U);
        }
        else
            CHECK(result.contents[0] == '\0' && !result.has_range);
    }
    else
    {
        UmiStatus status =
            envelope ? UmiLanguageHoverDocumentReadResponse(json, strlen(json), 7U, cancel, &document)
                     : UmiLanguageHoverDocumentCreate(json, strlen(json), cancel, &document);
        CHECK(status == wanted);
        if (status == UMI_STATUS_OK)
        {
            if (strcmp(mode, "ownership") == 0)
            {
                memset(owned, 'z', strlen(owned));
                free(owned);
                owned = NULL;
            }
            const char *text = NULL;
            size_t bytes = 0U;
            CHECK(UmiLanguageHoverDocumentText(document, &text, &bytes) == UMI_STATUS_OK);
            CHECK(bytes == strlen(expected) && strcmp(text, expected) == 0 &&
                  UmiLanguageHoverDocumentCount(document) == count);
            UmiLanguageHoverBlock block;
            if (count != 0U)
            {
                CHECK(UmiLanguageHoverDocumentAt(document, 0U, &block) == UMI_STATUS_OK);
                CHECK(block.kind == kind && strcmp(block.language, language) == 0);
            }
            CHECK(UmiLanguageHoverDocumentAt(document, count, &block) == UMI_STATUS_NOT_FOUND);
            UmiEditorTextPosition start = {99U, 98U}, end = {97U, 96U};
            CHECK(UmiLanguageHoverDocumentRange(document, &start, &end) ==
                  (has_range ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND));
            if (has_range)
                CHECK(start.line == 1U && start.utf16_column == 2U && end.line == 3U &&
                      end.utf16_column == 4U);
            else
                CHECK(start.line == 99U && end.line == 97U);
        }
        else
            CHECK(document == NULL);
    }
    UmiLanguageHoverDocumentDestroy(document);
    umi_cancellation_token_destroy(cancel);
    free(owned);
    return 0;
}
