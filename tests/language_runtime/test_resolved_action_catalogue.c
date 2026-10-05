/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_resolved_action_catalogue.c
 * PURPOSE: Check complete action resolution, metadata identity, independent ownership and rejection without partial publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/code_action_catalogue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
typedef struct Vector
{
    const char *name, *before, *response;
    UmiStatus status;
} Vector;
static const Vector vectors[] = {
    {"valid",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_OK},
    {"reordered",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}},\"extension\":{\"label\":\"café\"},\"diagnostics\":[],\"isPreferred\":true,"
     "\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"kind\":\"quickfix\",\"title\":\"Fix name\"}}",
     UMI_STATUS_OK},
    {"owned-input",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_OK},
    {"no-data", "[{\"title\":\"Fix name\"},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_OK},
    {"empty-edit",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"edit\":{}}}",
     UMI_STATUS_OK},
    {"changed-title",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Different\",\"kind\":\"quickfix\",\"data\":{"
     "\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,\"diagnostics\":[],\"extension\":{"
     "\"label\":\"café\"},\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_INVALID_STATE},
    {"changed-kind",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"refactor\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_INVALID_STATE},
    {"changed-data",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":2},\"isPreferred\":true,\"diagnostics\":[],\"extension\":"
     "{\"label\":\"café\"},\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_INVALID_STATE},
    {"changed-preferred",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":false,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_INVALID_STATE},
    {"changed-diagnostics",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[{}],\"extension\":{\"label\":\"café\"},\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_INVALID_STATE},
    {"changed-extension",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"other\"},\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_INVALID_STATE},
    {"missing-title",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\","
     "true,null]},\"isPreferred\":true,\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"edit\":{"
     "\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_INVALID_STATE},
    {"missing-data",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"isPreferred\":true,\"diagnostics\":[],\"extension\":{\"label\":\"café\"}"
     ",\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_INVALID_STATE},
    {"missing-extension",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_INVALID_STATE},
    {"missing-edit",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}}}",
     UMI_STATUS_INVALID_STATE},
    {"added-command",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}},\"command\":{\"title\":\"Run\",\"command\":\"hidden\"}}}",
     UMI_STATUS_INVALID_STATE},
    {"added-data", "[{\"title\":\"Fix name\"},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"data\":1,\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_INVALID_STATE},
    {"edit-null",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"edit\":null}}",
     UMI_STATUS_PARSE_ERROR},
    {"edit-array",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"edit\":[]}}",
     UMI_STATUS_PARSE_ERROR},
    {"result-null",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":null}", UMI_STATUS_PARSE_ERROR},
    {"result-array",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}]}",
     UMI_STATUS_PARSE_ERROR},
    {"disabled",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"disabled\":{\"reason\":\"blocked\"}},{"
     "\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_PERMISSION_DENIED},
    {"command",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"command\":{\"title\":\"Run\",\"command\":"
     "\"required\"}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_NOT_IMPLEMENTED},
    {"existing-edit",
     "[{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}},{\"title\":\"Other\",\"edit\":{}}]",
     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"Fix "
     "name\",\"kind\":\"quickfix\",\"data\":{\"id\":1,\"values\":[\"a\",true,null]},\"isPreferred\":true,"
     "\"diagnostics\":[],\"extension\":{\"label\":\"café\"},\"edit\":{\"changes\":{\"file:///workspace/"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"renamed\"}]}}}}",
     UMI_STATUS_INVALID_STATE}};
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const Vector *vector = &vectors[0];
    int known = 0;
    const char *special[] = {
        "escaped-text",   "numeric-spelling", "duplicate-data", "nested-duplicate", "index",
        "null-catalogue", "null-json",        "null-output",    "wrong-id",         "error",
        "cancelled",      "oversized",        "field-limit"};
    for (size_t i = 0U; i < sizeof(special) / sizeof(special[0]); ++i)
        if (strcmp(mode, special[i]) == 0)
            known = 1;
    for (size_t i = 0U; i < sizeof(vectors) / sizeof(vectors[0]); ++i)
        if (strcmp(mode, vectors[i].name) == 0)
        {
            vector = &vectors[i];
            known = 1;
        }
    CHECK(known);
    UmiStatus wanted = vector->status;
    const char *before = vector->before, *response = vector->response;
    if (strcmp(mode, "escaped-text") == 0)
    {
        before = "[{\"title\":\"a\",\"data\":{\"b\":2,\"a\":1}}]";
        response = "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"data\":{\"a\":1,\"b\":2},\"edit\":{},"
                   "\"title\":\"\\u0061\"}}";
    }
    if (strcmp(mode, "numeric-spelling") == 0)
    {
        before = "[{\"title\":\"a\",\"data\":1}]";
        response = "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"a\",\"data\":1.0,\"edit\":{}}}";
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "duplicate-data") == 0)
    {
        before = "[{\"title\":\"a\",\"data\":1,\"extra\":1}]";
        response =
            "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"a\",\"data\":1,\"data\":1,\"edit\":{}}}";
        wanted = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(mode, "nested-duplicate") == 0)
    {
        before = "[{\"title\":\"a\",\"data\":{\"x\":1,\"x\":1}}]";
        response = "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"title\":\"a\",\"data\":{\"x\":1,\"x\":1},"
                   "\"edit\":{}}}";
        wanted = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(mode, "error") == 0)
    {
        response = "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":-32603,\"message\":\"no\"}}";
        wanted = UMI_STATUS_UNAVAILABLE;
    }
    char *large = NULL, *fields = NULL, *reply = NULL;
    if (strcmp(mode, "field-limit") == 0)
    {
        fields = malloc(10000U);
        reply = malloc(11000U);
        CHECK(fields != NULL && reply != NULL);
        strcpy(fields, "[{\"title\":\"a\"");
        size_t used = strlen(fields);
        for (size_t i = 0U; i < 255U; ++i)
        {
            int n = snprintf(fields + used, 10000U - used, ",\"field%zu\":1", i);
            CHECK(n > 0 && (size_t)n < 10000U - used);
            used += (size_t)n;
        }
        strcpy(fields + used, "}]");
        int n = snprintf(reply, 11000U, "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":%.*s,\"edit\":{}}}",
                         (int)(used - 1U), fields + 1);
        CHECK(n > 0 && n < 11000);
        before = fields;
        response = reply;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiLanguageCodeActionCatalogue *source = NULL;
    CHECK(UmiLanguageCodeActionCatalogueCreate(before, strlen(before), NULL, &source) == UMI_STATUS_OK);
    size_t count = UmiLanguageCodeActionCatalogueCount(source), index = 0U, bytes = strlen(response);
    uint64_t id = 7U;
    if (strcmp(mode, "owned-input") == 0)
    {
        large = malloc(bytes + 1U);
        CHECK(large != NULL);
        memcpy(large, response, bytes + 1U);
        response = large;
    }
    if (strcmp(mode, "oversized") == 0)
    {
        bytes = 1024U * 1024U + 1U;
        large = calloc(bytes + 1U, 1U);
        CHECK(large != NULL);
        response = large;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "index") == 0)
    {
        index = count;
        wanted = UMI_STATUS_NOT_FOUND;
    }
    if (strncmp(mode, "null-", 5U) == 0)
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "wrong-id") == 0)
    {
        id = 8U;
        wanted = UMI_STATUS_NOT_FOUND;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        wanted = UMI_STATUS_CANCELLED;
    }
    UmiLanguageCodeActionCatalogue *result = NULL;
    CHECK(UmiLanguageCodeActionCatalogueWithResolvedResponse(
              strcmp(mode, "null-catalogue") == 0 ? NULL : source, index,
              strcmp(mode, "null-json") == 0 ? NULL : response, bytes, id, cancel,
              strcmp(mode, "null-output") == 0 ? NULL : &result) == wanted);
    free(large);
    free(fields);
    free(reply);
    CHECK(UmiLanguageCodeActionCatalogueCount(source) == count);
    if (wanted == UMI_STATUS_OK)
    {
        CHECK(result != NULL && UmiLanguageCodeActionCatalogueCount(result) == count);
        UmiLanguageCodeAction old, current;
        CHECK(UmiLanguageCodeActionCatalogueAt(source, 0U, &old) == UMI_STATUS_OK && !old.has_edit);
        UmiLanguageCodeActionCatalogueDestroy(source);
        source = NULL;
        CHECK(UmiLanguageCodeActionCatalogueAt(result, 0U, &current) == UMI_STATUS_OK && current.has_edit &&
              !current.has_command);
        UmiLanguageWorkspaceEditCatalogue *edits = NULL;
        CHECK(UmiLanguageCodeActionCatalogueReadEdits(result, 0U, NULL, &edits) == UMI_STATUS_OK);
        UmiLanguageWorkspaceEditCatalogueDestroy(edits);
        if (count == 2U)
        {
            CHECK(UmiLanguageCodeActionCatalogueAt(result, 1U, &current) == UMI_STATUS_OK &&
                  strcmp(current.title, "Other") == 0 && current.has_edit);
        }
    }
    else
        CHECK(result == NULL);
    UmiLanguageCodeActionCatalogueDestroy(source);
    UmiLanguageCodeActionCatalogueDestroy(result);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
