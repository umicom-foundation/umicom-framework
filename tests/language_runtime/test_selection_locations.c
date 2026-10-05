/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_selection_locations.c
 * PURPOSE: Check complete enclosing source ranges, cardinality, containment and atomic ownership.
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
typedef struct Case
{
    const char *name, *json;
    UmiStatus wanted;
    size_t count;
} Case;
static const Case cases[] = {
    {"single",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{"
     "\"line\":0,\"character\":2}}}]}",
     UMI_STATUS_OK, 1U},
    {"parents",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{"
     "\"line\":0,\"character\":2}},\"parent\":{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":4}},\"parent\":{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":2,\"character\":0}}}}}]}",
     UMI_STATUS_OK, 3U},
    {"null", "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":null}", UMI_STATUS_OK, 0U},
    {"empty-array", "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[]}", UMI_STATUS_PARSE_ERROR, 0U},
    {"multiple",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{"
     "\"line\":0,\"character\":2}}},{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,"
     "\"character\":2}}}]}",
     UMI_STATUS_PARSE_ERROR, 0U},
    {"object",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{"
     "\"line\":0,\"character\":2}}}}",
     UMI_STATUS_PARSE_ERROR, 0U},
    {"null-row", "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[null]}", UMI_STATUS_PARSE_ERROR, 0U},
    {"missing-range", "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{}]}", UMI_STATUS_PARSE_ERROR, 0U},
    {"missing-start",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"end\":{\"line\":0,\"character\":2}}}]}",
     UMI_STATUS_PARSE_ERROR, 0U},
    {"reversed",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":2},\"end\":{"
     "\"line\":0,\"character\":0}}}]}",
     UMI_STATUS_PARSE_ERROR, 0U},
    {"negative",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":-1},\"end\":{"
     "\"line\":0,\"character\":2}}}]}",
     UMI_STATUS_PARSE_ERROR, 0U},
    {"fraction",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":1.5}}}]}",
     UMI_STATUS_PARSE_ERROR, 0U},
    {"caret-outside",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":2},\"end\":{"
     "\"line\":0,\"character\":3}}}]}",
     UMI_STATUS_PARSE_ERROR, 0U},
    {"empty-at-caret",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{"
     "\"line\":0,\"character\":1}}}]}",
     UMI_STATUS_OK, 1U},
    {"caret-at-end",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":1}}}]}",
     UMI_STATUS_OK, 1U},
    {"parent-shrinks",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"parent\":{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{"
     "\"line\":0,\"character\":2}}}}]}",
     UMI_STATUS_PARSE_ERROR, 0U},
    {"parent-null",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":4}},\"parent\":null}]}",
     UMI_STATUS_PARSE_ERROR, 0U},
    {"parent-string",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":4}},\"parent\":\"parent\"}]}",
     UMI_STATUS_PARSE_ERROR, 0U},
    {"equal-parent",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":4}},\"parent\":{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":4}}}}]}",
     UMI_STATUS_OK, 2U},
    {"extension",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":4}},\"extension\":{\"value\":\"kept outside navigation metadata\"}}]}",
     UMI_STATUS_OK, 1U}};
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const Case *selected = NULL;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i].name) == 0)
            selected = &cases[i];
    const char *special[] = {"owned-uri", "invalid-uri", "null-uri",  "invalid-position", "null-output",
                             "wrong-id",  "error",       "cancelled", "depth-limit",      "depth-over"};
    int known = selected != NULL;
    for (size_t i = 0U; i < sizeof(special) / sizeof(special[0]); ++i)
        if (strcmp(mode, special[i]) == 0)
            known = 1;
    CHECK(known);
    const char *json = selected != NULL ? selected->json : cases[0].json;
    UmiStatus expected = selected != NULL ? selected->wanted : UMI_STATUS_OK;
    size_t count = selected != NULL ? selected->count : 1U;
    char uri[64] = "file:///workspace/main.c";
    UmiEditorTextPosition position = {0U, 1U};
    uint64_t id = 7U;
    if (strcmp(mode, "wrong-id") == 0)
    {
        id = 8U;
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "error") == 0)
    {
        json = "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":-1,\"message\":\"unavailable\"}}";
        expected = UMI_STATUS_UNAVAILABLE;
    }
    if (strcmp(mode, "invalid-uri") == 0)
    {
        strcpy(uri, "file:///bad%uri");
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "null-uri") == 0 || strcmp(mode, "null-output") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "invalid-position") == 0)
    {
        position.line = (uint64_t)INT32_MAX + 1U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Equal enclosing ranges are valid. Construct the boundary chain without
     * recursion or a large static response duplicated for every regression. */
    char chain[16384];
    if (strcmp(mode, "depth-limit") == 0 || strcmp(mode, "depth-over") == 0)
    {
        size_t depth = strcmp(mode, "depth-limit") == 0 ? 64U : 65U;
        strcpy(chain, "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[");
        const char *part =
            "{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":2}}";
        for (size_t i = 0U; i < depth; ++i)
        {
            CHECK(strlen(chain) + strlen(part) + 16U < sizeof(chain));
            strcat(chain, part);
            if (i + 1U < depth)
                strcat(chain, ",\"parent\":");
        }
        for (size_t i = 0U; i < depth; ++i)
            strcat(chain, "}");
        strcat(chain, "]}");
        json = chain;
        count = depth;
        if (depth > 64U)
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    UmiLanguageLocationCatalogue *catalogue = NULL;
    CHECK(UmiLanguageLocationCatalogueReadSelectionResponse(
              json, strlen(json), id, strcmp(mode, "null-uri") == 0 ? NULL : uri, position, cancel,
              strcmp(mode, "null-output") == 0 ? NULL : &catalogue) == expected);
    if (expected == UMI_STATUS_OK)
    {
        memset(uri, 'x', strlen(uri));
        CHECK(catalogue != NULL && UmiLanguageLocationCatalogueCount(catalogue) == count);
        for (size_t i = 0U; i < count; ++i)
        {
            UmiLanguageSourceLocation item;
            CHECK(UmiLanguageLocationCatalogueAt(catalogue, i, &item) == UMI_STATUS_OK);
            CHECK(strcmp(item.uri, "file:///workspace/main.c") == 0 && !item.is_link && !item.has_origin);
            CHECK(item.target.start.line == item.selection.start.line &&
                  item.target.end.utf16_column == item.selection.end.utf16_column);
            if (strcmp(mode, "parents") == 0)
                CHECK(item.selection.end.line == (i == 2U ? 2U : 0U) &&
                      item.selection.start.utf16_column == (i == 0U ? 1U : 0U));
        }
    }
    else
        CHECK(catalogue == NULL);
    UmiLanguageLocationCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
