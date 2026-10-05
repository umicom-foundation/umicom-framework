/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_completion_response.c
 * PURPOSE: Verify completion envelope identity and borrowed JSON value boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/completion_catalogue.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition)                                                                                     \
    do                                                                                                       \
    {                                                                                                        \
        if (!(condition))                                                                                    \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #condition);                                          \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
typedef struct ResponseCase
{
    const char *name, *json;
    UmiStatus status;
} ResponseCase;
static const ResponseCase cases[] = {
    {"array", "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"label\":\"x\"}]}", UMI_STATUS_OK},
    {"list", "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"isIncomplete\":false,\"items\":[]}}",
     UMI_STATUS_OK},
    {"null", "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":null}", UMI_STATUS_OK},
    {"wrong-id", "{\"jsonrpc\":\"2.0\",\"id\":8,\"result\":[]}", UMI_STATUS_NOT_FOUND},
    {"string-id", "{\"jsonrpc\":\"2.0\",\"id\":\"7\",\"result\":[]}", UMI_STATUS_PARSE_ERROR},
    {"missing-id", "{\"jsonrpc\":\"2.0\",\"result\":[]}", UMI_STATUS_PARSE_ERROR},
    {"missing-protocol", "{\"id\":7,\"result\":[]}", UMI_STATUS_PARSE_ERROR},
    {"protocol", "{\"jsonrpc\":\"1.0\",\"id\":7,\"result\":[]}", UMI_STATUS_PARSE_ERROR},
    {"duplicate-id", "{\"jsonrpc\":\"2.0\",\"id\":7,\"id\":7,\"result\":[]}", UMI_STATUS_ALREADY_EXISTS},
    {"mixed", "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[],\"error\":{}}", UMI_STATUS_PARSE_ERROR},
    {"request", "{\"jsonrpc\":\"2.0\",\"id\":7,\"method\":\"x\",\"result\":[]}", UMI_STATUS_PARSE_ERROR},
    {"error", "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":-32603,\"message\":\"unavailable\"}}",
     UMI_STATUS_UNAVAILABLE},
    {"error-code", "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":1.5,\"message\":\"bad\"}}",
     UMI_STATUS_PARSE_ERROR},
    {"error-message", "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":-1,\"message\":false}}",
     UMI_STATUS_PARSE_ERROR},
    {"missing-result", "{\"jsonrpc\":\"2.0\",\"id\":7}", UMI_STATUS_PARSE_ERROR}};
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    if (strcmp(name, "source-spans") == 0)
    {
        const char json[] = " { \"s\":\"\\u0061\", \"a\":[1, true], \"n\":null, \"o\":{} } ";
        UmiJsonTree *tree = NULL;
        CHECK(UmiJsonTreeCreate(json, strlen(json), NULL, NULL, &tree) == UMI_STATUS_OK);
        const char *names[] = {"s", "a", "n", "o"};
        const char *spellings[] = {"\"\\u0061\"", "[1, true]", "null", "{}"};
        for (size_t i = 0U; i < 4U; ++i)
        {
            int node;
            const char *bytes = NULL;
            size_t length = 0U;
            CHECK(UmiJsonTreeMember(tree, 0, names[i], &node) == UMI_STATUS_OK);
            CHECK(UmiJsonTreeSourceSpan(tree, node, &bytes, &length) == UMI_STATUS_OK);
            CHECK(length == strlen(spellings[i]) && memcmp(bytes, spellings[i], length) == 0);
        }
        const char *bytes = "unchanged";
        size_t length = 13U;
        CHECK(UmiJsonTreeSourceSpan(tree, -1, &bytes, &length) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(bytes, "unchanged") == 0 && length == 13U);
        UmiJsonTreeDestroy(tree);
    }
    else
    {
        UmiLanguageCompletionCatalogue *catalogue = NULL;
        if (strcmp(name, "arguments") == 0)
        {
            CHECK(UmiLanguageCompletionCatalogueReadResponse("{}", 2U, 0U, NULL, &catalogue) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLanguageCompletionCatalogueReadResponse("{}", 2U, UINT64_MAX, NULL, &catalogue) ==
                  UMI_STATUS_INVALID_ARGUMENT);
        }
        else if (strcmp(name, "cancel") == 0)
        {
            UmiCancellationToken *cancel = NULL;
            CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
            umi_cancellation_token_request(cancel);
            CHECK(UmiLanguageCompletionCatalogueReadResponse(cases[0].json, strlen(cases[0].json), 7U, cancel,
                                                             &catalogue) == UMI_STATUS_CANCELLED);
            umi_cancellation_token_destroy(cancel);
        }
        else
        {
            size_t i;
            for (i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
                if (strcmp(name, cases[i].name) == 0)
                    break;
            CHECK(i < sizeof(cases) / sizeof(cases[0]));
            CHECK(UmiLanguageCompletionCatalogueReadResponse(cases[i].json, strlen(cases[i].json), 7U, NULL,
                                                             &catalogue) == cases[i].status);
            if (cases[i].status == UMI_STATUS_OK)
                CHECK(catalogue != NULL);
            else
                CHECK(catalogue == NULL);
        }
        UmiLanguageCompletionCatalogueDestroy(catalogue);
    }
    return 0;
}
