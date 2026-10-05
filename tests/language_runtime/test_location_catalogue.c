/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_location_catalogue.c
 * PURPOSE: Check complete owned locations, link containment, strict boundaries and compatibility projection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/location_catalogue.h"
#include "umicom/language_runtime/decoders/locations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c);                                          \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
#define RANGE "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}}"
#define ITEM "{\"uri\":\"file:///workspace/main.c\",\"range\":" RANGE "}"
#define LINK                                                                                                 \
    "{\"targetUri\":\"file:///workspace/main.c\",\"targetRange\":" RANGE                                     \
    ",\"targetSelectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":2}" \
    "}}"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"single",
                           "array",
                           "null",
                           "empty",
                           "link",
                           "origin",
                           "selection-outside",
                           "missing-target",
                           "reversed",
                           "negative",
                           "fraction",
                           "limit-position",
                           "duplicate",
                           "ambiguous",
                           "mixed",
                           "invalid-item",
                           "uri-space",
                           "uri-scheme",
                           "uri-percent",
                           "uri-unicode",
                           "uri-limit",
                           "capacity",
                           "large",
                           "ownership",
                           "response",
                           "wrong-id",
                           "error",
                           "cancelled",
                           "arguments",
                           "legacy",
                           "legacy-link",
                           "legacy-capacity",
                           "legacy-invalid",
                           "legacy-uri-capacity"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    const char *json = ITEM, *uri = "file:///workspace/main.c";
    size_t count = 1U;
    int link = 0, origin = 0, response = 0, legacy = 0;
    UmiStatus expected = UMI_STATUS_OK;
    char *owned = NULL;
    if (strcmp(mode, "array") == 0)
    {
        json = "[" ITEM "," ITEM "]";
        count = 2U;
    }
    if (strcmp(mode, "null") == 0 || strcmp(mode, "empty") == 0)
    {
        json = strcmp(mode, "null") == 0 ? "null" : "[]";
        count = 0U;
    }
    if (strcmp(mode, "link") == 0)
    {
        json = "[" LINK "]";
        link = 1;
    }
    if (strcmp(mode, "origin") == 0)
    {
        json = "[{\"targetUri\":\"file:///workspace/main.c\",\"targetRange\":" RANGE
               ",\"targetSelectionRange\":" RANGE ",\"originSelectionRange\":" RANGE "}]";
        link = 1;
        origin = 1;
    }
    if (strcmp(mode, "selection-outside") == 0)
    {
        json = "[{\"targetUri\":\"file:///workspace/main.c\",\"targetRange\":" RANGE
               ",\"targetSelectionRange\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":1,"
               "\"character\":0}}}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "missing-target") == 0)
    {
        json = "[{\"targetUri\":\"file:///workspace/main.c\",\"targetSelectionRange\":" RANGE "}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "reversed") == 0)
    {
        json = "{\"uri\":\"file:///"
               "x\",\"range\":{\"start\":{\"line\":1,\"character\":0},\"end\":{\"line\":0,\"character\":0}}}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "negative") == 0)
    {
        json =
            "{\"uri\":\"file:///"
            "x\",\"range\":{\"start\":{\"line\":-1,\"character\":0},\"end\":{\"line\":0,\"character\":0}}}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "fraction") == 0)
    {
        json =
            "{\"uri\":\"file:///"
            "x\",\"range\":{\"start\":{\"line\":0.5,\"character\":0},\"end\":{\"line\":1,\"character\":0}}}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "limit-position") == 0)
    {
        json = "{\"uri\":\"file:///"
               "x\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":2147483648,"
               "\"character\":0}}}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "duplicate") == 0)
    {
        json = "{\"uri\":\"file:///a\",\"uri\":\"file:///b\",\"range\":" RANGE "}";
        expected = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(mode, "ambiguous") == 0)
    {
        json = "{\"uri\":\"file:///a\",\"targetUri\":\"file:///b\",\"range\":" RANGE "}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "mixed") == 0)
    {
        json = "[" ITEM "," LINK "]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "invalid-item") == 0)
    {
        json = "[" ITEM ",{}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "uri-space") == 0)
    {
        json = "{\"uri\":\"file:///space name\",\"range\":" RANGE "}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "uri-scheme") == 0)
    {
        json = "{\"uri\":\"relative.c\",\"range\":" RANGE "}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "uri-percent") == 0)
    {
        json = "{\"uri\":\"file:///invalid%xy\",\"range\":" RANGE "}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "uri-unicode") == 0)
    {
        json = "{\"uri\":\"file:///caf\\u00e9%20name.c\",\"range\":" RANGE "}";
        uri = "file:///caf\xc3\xa9%20name.c";
    }
    if (strcmp(mode, "uri-limit") == 0 || strcmp(mode, "legacy-uri-capacity") == 0)
    {
        size_t length = strcmp(mode, "uri-limit") == 0 ? 8193U : UMI_LANGUAGE_RUNTIME_PATH_CAPACITY;
        owned = malloc(length + 256U);
        CHECK(owned != NULL);
        strcpy(owned, "{\"uri\":\"file:///");
        size_t prefix = strlen(owned);
        memset(owned + prefix, 'x', length - 8U);
        strcpy(owned + prefix + length - 8U, "\",\"range\":" RANGE "}");
        json = owned;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
        legacy = strcmp(mode, "legacy-uri-capacity") == 0;
    }
    if (strcmp(mode, "capacity") == 0 || strcmp(mode, "large") == 0 || strcmp(mode, "legacy-capacity") == 0)
    {
        count = strcmp(mode, "capacity") == 0 ? 4097U : strcmp(mode, "large") == 0 ? 4096U : 257U;
        size_t item_bytes = strlen(ITEM);
        owned = malloc((item_bytes + 1U) * count + 3U);
        CHECK(owned != NULL);
        size_t offset = 0U;
        owned[offset++] = '[';
        for (size_t i = 0U; i < count; ++i)
        {
            if (i != 0U)
                owned[offset++] = ',';
            memcpy(owned + offset, ITEM, item_bytes);
            offset += item_bytes;
        }
        owned[offset++] = ']';
        owned[offset] = '\0';
        json = owned;
        expected = strcmp(mode, "large") == 0 ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
        legacy = strcmp(mode, "legacy-capacity") == 0;
    }
    if (strcmp(mode, "ownership") == 0)
    {
        owned = malloc(strlen(json) + 1U);
        CHECK(owned != NULL);
        strcpy(owned, json);
        json = owned;
    }
    if (strcmp(mode, "response") == 0)
    {
        json = "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":" ITEM "}";
        response = 1;
    }
    if (strcmp(mode, "wrong-id") == 0)
    {
        json = "{\"jsonrpc\":\"2.0\",\"id\":8,\"result\":" ITEM "}";
        response = 1;
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "error") == 0)
    {
        json = "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":-1,\"message\":\"refused\"}}";
        response = 1;
        expected = UMI_STATUS_UNAVAILABLE;
    }
    if (strcmp(mode, "legacy") == 0)
        legacy = 1;
    if (strcmp(mode, "legacy-link") == 0)
    {
        json = LINK;
        legacy = link = 1;
    }
    if (strcmp(mode, "legacy-invalid") == 0)
    {
        json = "[" ITEM ",{}]";
        legacy = 1;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    UmiLanguageLocationCatalogue *catalogue = NULL;
    if (legacy)
    {
        size_t size = strlen(json) + 32U;
        char *envelope = malloc(size);
        CHECK(envelope != NULL);
        (void)snprintf(envelope, size, "{\"result\":%s}", json);
        UmiLanguageRuntimeLocationList *result = malloc(sizeof(*result));
        CHECK(result != NULL);
        memset(result, 0xa5, sizeof(*result));
        CHECK(umi_language_runtime_decode_locations(envelope, result) == expected);
        if (expected == UMI_STATUS_OK)
            CHECK(result->count == count && strcmp(result->items[0].uri, uri) == 0 &&
                  result->items[0].range.start.character == (link ? 1U : 0U));
        else
            CHECK(result->count == 0U && result->items[0].uri[0] == '\0');
        free(result);
        free(envelope);
    }
    else
    {
        UmiStatus status =
            response ? UmiLanguageLocationCatalogueReadResponse(json, strlen(json), 7U, cancel, &catalogue)
                     : UmiLanguageLocationCatalogueCreate(json, strlen(json), cancel, &catalogue);
        CHECK(status == expected);
        if (status == UMI_STATUS_OK)
        {
            if (strcmp(mode, "ownership") == 0)
            {
                memset(owned, 'z', strlen(owned));
                free(owned);
                owned = NULL;
            }
            CHECK(UmiLanguageLocationCatalogueCount(catalogue) == count);
            UmiLanguageSourceLocation location = {0};
            for (size_t i = 0U; i < count; ++i)
            {
                CHECK(UmiLanguageLocationCatalogueAt(catalogue, i, &location) == UMI_STATUS_OK);
                CHECK(strcmp(location.uri, uri) == 0 && location.is_link == link &&
                      location.has_origin == origin);
                CHECK(location.selection.start.utf16_column == (link && !origin ? 1U : 0U));
            }
            location.has_origin = 99;
            CHECK(UmiLanguageLocationCatalogueAt(catalogue, count, &location) == UMI_STATUS_NOT_FOUND &&
                  location.has_origin == 99);
        }
        else
            CHECK(catalogue == NULL);
    }
    if (strcmp(mode, "arguments") == 0)
    {
        UmiLanguageLocationCatalogue *bad = catalogue;
        CHECK(UmiLanguageLocationCatalogueCreate(NULL, 3U, NULL, &bad) == UMI_STATUS_INVALID_ARGUMENT &&
              bad == NULL);
        CHECK(UmiLanguageLocationCatalogueCreate("null", 4U, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLanguageLocationCatalogueAt(catalogue, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    UmiLanguageLocationCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    free(owned);
    return 0;
}
