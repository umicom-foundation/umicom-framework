/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_symbol_catalogue.c
 * PURPOSE: Check owned symbol hierarchy, flat compatibility, strict fields and bounded complete publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/symbol_catalogue.h"
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
#define START "{\"line\":0,\"character\":0}"
#define END "{\"line\":0,\"character\":4}"
#define RANGE "{\"start\":" START ",\"end\":" END "}"
#define CHILD "{\"name\":\"field\",\"kind\":8,\"range\":" RANGE ",\"selectionRange\":" RANGE "}"
#define SYMBOL "{\"name\":\"Type\",\"kind\":23,\"range\":" RANGE ",\"selectionRange\":" RANGE "}"
#define FLAT                                                                                                 \
    "{\"name\":\"flat\",\"kind\":12,\"containerName\":\"container\",\"location\":{\"uri\":\"file:///"        \
    "other.c\",\"range\":" RANGE "}}"
/* This builder changes one field's wire spelling independently of the reader. */
static char *Replace(const char *source, const char *before, const char *after)
{
    const char *at = strstr(source, before);
    if (at == NULL)
        return NULL;
    size_t prefix = (size_t)(at - source), tail = strlen(at + strlen(before)), extra = strlen(after);
    char *text = malloc(prefix + extra + tail + 1U);
    if (text != NULL)
    {
        memcpy(text, source, prefix);
        memcpy(text + prefix, after, extra);
        memcpy(text + prefix + extra, at + strlen(before), tail + 1U);
    }
    return text;
}
static char *Rows(size_t count)
{
    size_t row = strlen(SYMBOL), capacity = 2U + count * (row + 1U) + 1U;
    char *text = malloc(capacity);
    if (text == NULL)
        return NULL;
    size_t at = 0U;
    text[at++] = '[';
    for (size_t i = 0U; i < count; ++i)
    {
        if (i != 0U)
            text[at++] = ',';
        memcpy(text + at, SYMBOL, row);
        at += row;
    }
    text[at++] = ']';
    text[at] = '\0';
    return text;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"hierarchy",
                           "flat",
                           "empty",
                           "null",
                           "selection",
                           "owned",
                           "large",
                           "capacity",
                           "depth",
                           "depth-limit",
                           "name-empty",
                           "name-space",
                           "name-unicode-space",
                           "name-unicode",
                           "name-limit",
                           "detail",
                           "detail-type",
                           "container",
                           "kind-unknown",
                           "kind-zero",
                           "kind-fraction",
                           "tags",
                           "tag-unknown",
                           "tag-invalid",
                           "deprecated",
                           "deprecated-type",
                           "selection-outside",
                           "missing-selection",
                           "reversed",
                           "negative",
                           "mixed",
                           "child-flat",
                           "children-type",
                           "duplicate",
                           "uri",
                           "uri-quote",
                           "uri-invalid-utf8",
                           "flat-uri",
                           "response",
                           "wrong-id",
                           "error",
                           "cancelled",
                           "arguments"};
    int known_mode = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            known_mode = 1;
    CHECK(known_mode);
    const char *json = "[" SYMBOL "]", *uri = "file:///source.c";
    char *owned = NULL;
    UmiStatus expected = UMI_STATUS_OK;
    size_t count = 1U;
    if (strcmp(mode, "hierarchy") == 0)
    {
        json = "[{\"name\":\"Type\",\"kind\":23,\"range\":" RANGE ",\"selectionRange\":" RANGE
               ",\"children\":[" CHILD "]}]";
        count = 2U;
    }
    if (strcmp(mode, "flat") == 0 || strcmp(mode, "container") == 0)
        json = "[" FLAT "]";
    if (strcmp(mode, "empty") == 0 || strcmp(mode, "null") == 0)
    {
        json = strcmp(mode, "empty") == 0 ? "[]" : "null";
        count = 0U;
    }
    if (strcmp(mode, "large") == 0 || strcmp(mode, "capacity") == 0)
    {
        count = strcmp(mode, "large") == 0 ? 512U : 4097U;
        owned = Rows(count);
        CHECK(owned != NULL);
        json = owned;
        if (count > 4096U)
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "depth") == 0 || strcmp(mode, "depth-limit") == 0)
    {
        count = strcmp(mode, "depth") == 0 ? 32U : 33U;
        const char *prefix =
            "{\"name\":\"Type\",\"kind\":23,\"range\":" RANGE ",\"selectionRange\":" RANGE ",\"children\":[";
        size_t n = strlen(prefix);
        owned = malloc(count * (n + 2U) + 3U);
        CHECK(owned != NULL);
        size_t at = 0U;
        owned[at++] = '[';
        for (size_t i = 0U; i < count; ++i)
        {
            memcpy(owned + at, prefix, n);
            at += n;
        }
        for (size_t i = 0U; i < count; ++i)
        {
            owned[at++] = ']';
            owned[at++] = '}';
        }
        owned[at++] = ']';
        owned[at] = '\0';
        json = owned;
        if (count > 32U)
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    const char *before = NULL, *after = NULL;
    if (strcmp(mode, "name-empty") == 0)
    {
        before = "\"Type\"";
        after = "\"\"";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "name-space") == 0)
    {
        before = "\"Type\"";
        after = "\" \\t\\r\\n\"";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "name-unicode-space") == 0)
    {
        before = "\"Type\"";
        after = "\"\\u00a0\\u2003\\u3000\"";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "name-unicode") == 0)
    {
        before = "\"Type\"";
        after = "\"caf\\u00e9\"";
    }
    if (strcmp(mode, "detail") == 0)
    {
        before = "\"kind\":23";
        after = "\"kind\":23,\"detail\":\"class Type\"";
    }
    if (strcmp(mode, "detail-type") == 0)
    {
        before = "\"kind\":23";
        after = "\"kind\":23,\"detail\":7";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "kind-unknown") == 0)
    {
        before = "\"kind\":23";
        after = "\"kind\":500";
    }
    if (strcmp(mode, "kind-zero") == 0)
    {
        before = "\"kind\":23";
        after = "\"kind\":0";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "kind-fraction") == 0)
    {
        before = "\"kind\":23";
        after = "\"kind\":2.3";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "tags") == 0)
    {
        before = "\"kind\":23";
        after = "\"kind\":23,\"tags\":[1]";
    }
    if (strcmp(mode, "tag-unknown") == 0)
    {
        before = "\"kind\":23";
        after = "\"kind\":23,\"tags\":[123]";
    }
    if (strcmp(mode, "tag-invalid") == 0)
    {
        before = "\"kind\":23";
        after = "\"kind\":23,\"tags\":[\"1\"]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "deprecated") == 0)
    {
        before = "\"kind\":23";
        after = "\"kind\":23,\"deprecated\":true";
    }
    if (strcmp(mode, "deprecated-type") == 0)
    {
        before = "\"kind\":23";
        after = "\"kind\":23,\"deprecated\":1";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "selection") == 0)
    {
        before = "\"selectionRange\":" RANGE;
        after = "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,"
                "\"character\":2}}";
    }
    if (strcmp(mode, "selection-outside") == 0)
    {
        before = "\"selectionRange\":" RANGE;
        after = "\"selectionRange\":{\"start\":" START ",\"end\":{\"line\":0,\"character\":5}}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "missing-selection") == 0)
    {
        before = ",\"selectionRange\":" RANGE;
        after = "";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "reversed") == 0)
    {
        before = "\"range\":" RANGE;
        after = "\"range\":{\"start\":" END ",\"end\":" START "}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "negative") == 0)
    {
        before = "\"line\":0";
        after = "\"line\":-1";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "children-type") == 0)
    {
        before = "\"kind\":23";
        after = "\"kind\":23,\"children\":{}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "duplicate") == 0)
    {
        before = "\"kind\":23";
        after = "\"kind\":23,\"kind\":5";
        expected = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(mode, "mixed") == 0)
    {
        json = "[" SYMBOL "," FLAT "]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "child-flat") == 0)
    {
        json = "[{\"name\":\"Type\",\"kind\":23,\"range\":" RANGE ",\"selectionRange\":" RANGE
               ",\"children\":[" FLAT "]}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "uri") == 0)
    {
        uri = "relative/path";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "uri-quote") == 0)
    {
        uri = "file:///a\"b";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "uri-invalid-utf8") == 0)
    {
        uri = "file:///a\xc0\x80";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "flat-uri") == 0)
    {
        json = "[" FLAT "]";
        before = "file:///other.c";
        after = "file:///bad%GG";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (before != NULL)
    {
        owned = Replace(json, before, after);
        CHECK(owned != NULL);
        json = owned;
    }
    if (strcmp(mode, "name-limit") == 0)
    {
        char name[4100];
        name[0] = '"';
        memset(name + 1U, 'x', 4097U);
        name[4098] = '"';
        name[4099] = '\0';
        owned = Replace(json, "\"Type\"", name);
        CHECK(owned != NULL);
        json = owned;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "owned") == 0)
    {
        owned = malloc(strlen(json) + 1U);
        CHECK(owned != NULL);
        strcpy(owned, json);
        json = owned;
    }
    UmiCancellationToken *cancel = NULL;
    if (strcmp(mode, "cancelled") == 0)
    {
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    UmiLanguageSymbolCatalogue *catalogue = NULL;
    UmiStatus status;
    int response =
        strcmp(mode, "response") == 0 || strcmp(mode, "wrong-id") == 0 || strcmp(mode, "error") == 0;
    if (response)
    {
        const char *reply =
            strcmp(mode, "error") == 0
                ? "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":-32603,\"message\":\"refused\"}}"
                : "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[" SYMBOL "]}";
        if (strcmp(mode, "wrong-id") == 0)
            expected = UMI_STATUS_NOT_FOUND;
        if (strcmp(mode, "error") == 0)
            expected = UMI_STATUS_UNAVAILABLE;
        status = UmiLanguageSymbolCatalogueReadResponse(
            reply, strlen(reply), strcmp(mode, "wrong-id") == 0 ? 8U : 7U, uri, cancel, &catalogue);
    }
    else
        status = UmiLanguageSymbolCatalogueCreate(json, strlen(json), uri, cancel, &catalogue);
    CHECK(status == expected);
    if (status == UMI_STATUS_OK)
    {
        if (strcmp(mode, "owned") == 0)
            memset(owned, '?', strlen(owned));
        CHECK(catalogue != NULL && UmiLanguageSymbolCatalogueCount(catalogue) == count);
        UmiLanguageSymbol symbol = {0};
        if (count != 0U)
        {
            CHECK(UmiLanguageSymbolCatalogueAt(catalogue, 0U, &symbol) == UMI_STATUS_OK &&
                  symbol.parent == SIZE_MAX && symbol.depth == 0U);
            if (strcmp(mode, "flat") == 0 || strcmp(mode, "container") == 0)
            {
                CHECK(!symbol.hierarchical && strcmp(symbol.container, "container") == 0 &&
                      strcmp(symbol.location.uri, "file:///other.c") == 0);
                CHECK(symbol.location.selection.end.utf16_column == 0U &&
                      symbol.location.target.end.utf16_column == 4U);
            }
            else
                CHECK(symbol.hierarchical && strcmp(symbol.location.uri, uri) == 0);
            if (strcmp(mode, "hierarchy") == 0)
            {
                CHECK(UmiLanguageSymbolCatalogueAt(catalogue, 1U, &symbol) == UMI_STATUS_OK);
                CHECK(symbol.parent == 0U && symbol.depth == 1U && strcmp(symbol.name, "field") == 0);
            }
            if (strcmp(mode, "depth") == 0)
            {
                CHECK(UmiLanguageSymbolCatalogueAt(catalogue, 31U, &symbol) == UMI_STATUS_OK);
                CHECK(symbol.depth == 31U && symbol.parent == 30U);
            }
            if (strcmp(mode, "owned") == 0)
                CHECK(strcmp(symbol.name, "Type") == 0);
            if (strcmp(mode, "name-unicode") == 0)
                CHECK(strcmp(symbol.name, "caf\xc3\xa9") == 0);
            if (strcmp(mode, "detail") == 0)
                CHECK(strcmp(symbol.detail, "class Type") == 0);
            if (strcmp(mode, "kind-unknown") == 0)
                CHECK(symbol.kind == 500);
            if (strcmp(mode, "tags") == 0 || strcmp(mode, "deprecated") == 0)
                CHECK(symbol.deprecated);
            if (strcmp(mode, "tag-unknown") == 0)
                CHECK(!symbol.deprecated);
            if (strcmp(mode, "selection") == 0)
                CHECK(symbol.location.selection.start.utf16_column == 1U &&
                      symbol.location.selection.end.utf16_column == 2U);
        }
        symbol.kind = 777;
        CHECK(UmiLanguageSymbolCatalogueAt(catalogue, count, &symbol) == UMI_STATUS_NOT_FOUND &&
              symbol.kind == 777);
        if (strcmp(mode, "arguments") == 0)
        {
            CHECK(UmiLanguageSymbolCatalogueAt(NULL, 0U, &symbol) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLanguageSymbolCatalogueAt(catalogue, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLanguageSymbolCatalogueCreate(json, strlen(json), uri, NULL, NULL) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            UmiLanguageSymbolCatalogue *none = catalogue;
            CHECK(UmiLanguageSymbolCatalogueCreate(json, strlen(json), NULL, NULL, &none) ==
                      UMI_STATUS_INVALID_ARGUMENT &&
                  none == NULL);
        }
    }
    else
        CHECK(catalogue == NULL);
    UmiLanguageSymbolCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    free(owned);
    return 0;
}
