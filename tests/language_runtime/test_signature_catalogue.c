/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_signature_catalogue.c
 * PURPOSE: Check complete overload ownership, active choices and Unicode parameter spans.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/signature_catalogue.h"
#include "umicom/language_runtime/decoders/signature.h"
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
typedef struct Example
{
    const char *name, *json;
    UmiStatus status;
    size_t count, active;
    int parameter, span;
    const char *label;
    size_t first, last;
} Example;
/* Independent protocol examples specify expected user-visible choices. */
static const Example examples[] = {
    {"valid",
     "{\"signatures\":[{\"label\":\"sum(left, right)\",\"documentation\":\"Add two "
     "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]}],\"activeParameter\":1}",
     UMI_STATUS_OK, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"null", "null", UMI_STATUS_OK, 0U, 0U, 0, 0, "", 0U, 0U},
    {"empty", "{\"signatures\":[]}", UMI_STATUS_OK, 0U, 0U, 1, 1, "right", 10U, 15U},
    {"overload",
     "{\"signatures\":[{\"label\":\"sum(left, right)\",\"documentation\":\"Add two "
     "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]},{\"label\":\"sum(values)\",\"parameters\":[{\"label\":\"values\"}]}],\"activeParameter\":"
     "1,\"activeSignature\":1}",
/* The overload input contains two signatures; index one selects the second. Keep both results in the catalogue and retain the earlier inconsistent count for review. */
#if 0
     UMI_STATUS_OK, 1U, 1U, 0, 1, "values", 4U, 10U},
#endif
     UMI_STATUS_OK, 2U, 1U, 0, 1, "values", 4U, 10U},
    {"local-active",
     "{\"signatures\":[{\"label\":\"sum(left, right)\",\"documentation\":\"Add two "
     "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}],\"activeParameter\":0}],\"activeParameter\":1}",
     UMI_STATUS_OK, 1U, 0U, 0, 1, "left", 4U, 8U},
    {"default-active",
     "{\"signatures\":[{\"label\":\"sum(left, right)\",\"documentation\":\"Add two "
     "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]}]}",
     UMI_STATUS_OK, 1U, 0U, 0, 1, "left", 4U, 8U},
    {"signature-fallback",
     "{\"signatures\":[{\"label\":\"sum(left, right)\",\"documentation\":\"Add two "
     "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]}],\"activeParameter\":1,\"activeSignature\":999}",
     UMI_STATUS_OK, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"parameter-fallback",
     "{\"signatures\":[{\"label\":\"sum(left, right)\",\"documentation\":\"Add two "
     "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]}],\"activeParameter\":999}",
     UMI_STATUS_OK, 1U, 0U, 0, 1, "left", 4U, 8U},
    {"local-fallback",
     "{\"signatures\":[{\"label\":\"sum(left, right)\",\"documentation\":\"Add two "
     "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}],\"activeParameter\":999}],\"activeParameter\":1}",
     UMI_STATUS_OK, 1U, 0U, 0, 1, "left", 4U, 8U},
    {"no-parameters", "{\"signatures\":[{\"label\":\"f()\"}]}", UMI_STATUS_OK, 1U, 0U, -1, 0, "", 10U, 15U},
    {"empty-parameters", "{\"signatures\":[{\"label\":\"f()\",\"parameters\":[]}]}", UMI_STATUS_OK, 1U, 0U,
     -1, 0, "", 10U, 15U},
    {"unicode", "{\"signatures\":[{\"label\":\"f(\\ud83d\\ude00)\",\"parameters\":[{\"label\":[2,4]}]}]}",
     UMI_STATUS_OK, 1U, 0U, 0, 1, "\360\237\230\200", 2U, 6U},
    {"multiline", "{\"signatures\":[{\"label\":\"f(\\r\\nvalue)\",\"parameters\":[{\"label\":[4,9]}]}]}",
     UMI_STATUS_OK, 1U, 0U, 0, 1, "value", 4U, 9U},
    {"zero-span", "{\"signatures\":[{\"label\":\"f()\",\"parameters\":[{\"label\":[2,2]}]}]}", UMI_STATUS_OK,
     1U, 0U, 0, 1, "", 2U, 2U},
    {"ambiguous", "{\"signatures\":[{\"label\":\"f(a, a)\",\"parameters\":[{\"label\":\"a\"}]}]}",
     UMI_STATUS_OK, 1U, 0U, 0, 0, "a", 0U, 0U},
    {"missing-substring", "{\"signatures\":[{\"label\":\"f(a)\",\"parameters\":[{\"label\":\"b\"}]}]}",
     UMI_STATUS_OK, 1U, 0U, 0, 0, "b", 0U, 0U},
    {"empty-string", "{\"signatures\":[{\"label\":\"f()\",\"parameters\":[{\"label\":\"\"}]}]}",
     UMI_STATUS_OK, 1U, 0U, 0, 0, "", 0U, 0U},
    {"split-surrogate",
     "{\"signatures\":[{\"label\":\"f(\\ud83d\\ude00)\",\"parameters\":[{\"label\":[2,3]}]}]}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"range-overflow",
     "{\"signatures\":[{\"label\":\"f(\\ud83d\\ude00)\",\"parameters\":[{\"label\":[2,5]}]}]}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"range-negative",
     "{\"signatures\":[{\"label\":\"f(\\ud83d\\ude00)\",\"parameters\":[{\"label\":[-1,2]}]}]}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"reversed", "{\"signatures\":[{\"label\":\"f(\\ud83d\\ude00)\",\"parameters\":[{\"label\":[4,2]}]}]}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"tuple-short", "{\"signatures\":[{\"label\":\"f(\\ud83d\\ude00)\",\"parameters\":[{\"label\":[2]}]}]}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"tuple-long",
     "{\"signatures\":[{\"label\":\"f(\\ud83d\\ude00)\",\"parameters\":[{\"label\":[2,4,5]}]}]}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"tuple-fraction",
     "{\"signatures\":[{\"label\":\"f(\\ud83d\\ude00)\",\"parameters\":[{\"label\":[2,3.5]}]}]}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"label-type", "{\"signatures\":[{\"label\":\"f(\\ud83d\\ude00)\",\"parameters\":[{\"label\":true}]}]}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"negative-active",
     "{\"signatures\":[{\"label\":\"sum(left, right)\",\"documentation\":\"Add two "
     "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]}],\"activeParameter\":1,\"activeSignature\":-1}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"active-fraction",
     "{\"signatures\":[{\"label\":\"sum(left, right)\",\"documentation\":\"Add two "
     "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]}],\"activeParameter\":1,\"activeSignature\":0.5}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"active-type",
     "{\"signatures\":[{\"label\":\"sum(left, right)\",\"documentation\":\"Add two "
     "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]}],\"activeParameter\":\"one\"}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"active-overflow",
     "{\"signatures\":[{\"label\":\"sum(left, right)\",\"documentation\":\"Add two "
     "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]}],\"activeParameter\":2147483648}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"parameters-type",
     "{\"signatures\":[{\"label\":\"sum(left, right)\",\"documentation\":\"Add two "
     "values.\",\"parameters\":{}}],\"activeParameter\":1}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"documentation-type",
     "{\"signatures\":[{\"label\":\"sum(left, "
     "right)\",\"documentation\":false,\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]}],\"activeParameter\":1}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"unknown-markup",
     "{\"signatures\":[{\"label\":\"sum(left, "
     "right)\",\"documentation\":{\"kind\":\"html\",\"value\":\"text\"},\"parameters\":[{\"label\":\"left\","
     "\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]}],\"activeParameter\":1}",
     UMI_STATUS_NOT_IMPLEMENTED, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"missing-markup",
     "{\"signatures\":[{\"label\":\"sum(left, "
     "right)\",\"documentation\":{\"kind\":\"plaintext\"},\"parameters\":[{\"label\":\"left\","
     "\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]}],\"activeParameter\":1}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"signature-label-type",
     "{\"signatures\":[{\"label\":false,\"documentation\":\"Add two "
     "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]}],\"activeParameter\":1}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"missing-label",
     "{\"signatures\":[{\"documentation\":\"Add two "
     "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]}],\"activeParameter\":1}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"signature-array-type", "{\"signatures\":{}}", UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"parameter-doc-type",
     "{\"signatures\":[{\"label\":\"sum(left, right)\",\"documentation\":\"Add two "
     "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":false},{\"label\":[10,15],"
     "\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** value.\"}}]}],\"activeParameter\":1}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 1, 1, "right", 10U, 15U},
    {"literal-documentation",
     "{\"signatures\":[{\"label\":\"sum(left, "
     "right)\",\"documentation\":{\"kind\":\"plaintext\",\"value\":\"<script>literal</"
     "script>\"},\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
     "value.\"},{\"label\":[10,15],\"documentation\":{\"kind\":\"markdown\",\"value\":\"**Second** "
     "value.\"}}]}],\"activeParameter\":1}",
     UMI_STATUS_OK, 1U, 0U, 1, 1, "right", 10U, 15U},

/* Duplicate addressed members use the shared tree status. Retain the earlier
 * expectation for review. */
#if 0
{"duplicate", "{\"signatures\":[],\"signatures\":[]}", UMI_STATUS_PARSE_ERROR, 0U, 0U, 0, 0, "", 0U, 0U},
#endif
    {"duplicate", "{\"signatures\":[],\"signatures\":[]}", UMI_STATUS_ALREADY_EXISTS, 0U, 0U, 0, 0, "", 0U,
     0U},
    {"embedded-nul", "{\"signatures\":[{\"label\":\"f\\u0000()\"}]}", UMI_STATUS_PARSE_ERROR, 0U, 0U, 0, 0,
     "", 0U, 0U},
};
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const Example *example = NULL;
    for (size_t i = 0U; i < sizeof(examples) / sizeof(examples[0]); ++i)
        if (strcmp(mode, examples[i].name) == 0)
            example = &examples[i];
    const char *json = example != NULL ? example->json : examples[0].json;
    UmiStatus expected = example != NULL ? example->status : UMI_STATUS_OK;
    char *owned = NULL;
    int response = 0, legacy = 0;
    if (strcmp(mode, "owned") == 0)
    {
        owned = malloc(strlen(json) + 1U);
        CHECK(owned != NULL);
        strcpy(owned, json);
        json = owned;
    }
    else if (strcmp(mode, "response") == 0 || strcmp(mode, "wrong-id") == 0 || strcmp(mode, "error") == 0 ||
             strcmp(mode, "legacy") == 0)
    {
        owned = malloc(strlen(json) + 160U);
        CHECK(owned != NULL);
        if (strcmp(mode, "error") == 0)
        {
            strcpy(owned,
                   "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":-32603,\"message\":\"failed\"}}");
            expected = UMI_STATUS_UNAVAILABLE;
        }
        else
            (void)sprintf(owned, "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":%s}", json);
        json = owned;
        response = 1;
        if (strcmp(mode, "wrong-id") == 0)
            expected = UMI_STATUS_NOT_FOUND;
        legacy = strcmp(mode, "legacy") == 0;
    }
    else if (strcmp(mode, "label-limit") == 0 || strcmp(mode, "legacy-limit") == 0)
    {
        size_t count = strcmp(mode, "label-limit") == 0 ? 65537U : 1100U;
        const char *prefix = strcmp(mode, "label-limit") == 0 ? "{\"signatures\":[{\"label\":\""
                                                              : "{\"result\":{\"signatures\":[{\"label\":\"";
        owned = malloc(count + 100U);
        CHECK(owned != NULL);
        strcpy(owned, prefix);
        size_t at = strlen(prefix);
        memset(owned + at, 'a', count);
        strcpy(owned + at + count, strcmp(mode, "label-limit") == 0 ? "\"}]}" : "\"}]}}");
        json = owned;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
        legacy = strcmp(mode, "legacy-limit") == 0;
    }
    else if (strcmp(mode, "expanded-label-limit") == 0)
    {
        owned = malloc(80000U);
        CHECK(owned != NULL);
        strcpy(owned, "{\"signatures\":[{\"label\":\"");
        size_t at = strlen(owned);
        memset(owned + at, 'a', 65536U);
        at += 65536U;
        at += (size_t)sprintf(owned + at, "\",\"parameters\":[");
        for (size_t i = 0U; i < 65U; ++i)
            at += (size_t)sprintf(owned + at, "%s{\"label\":[0,65536]}", i == 0U ? "" : ",");
        strcpy(owned + at, "]}]}");
        json = owned;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(mode, "signature-limit") == 0 || strcmp(mode, "parameter-limit") == 0 ||
             strcmp(mode, "total-limit") == 0)
    {
        owned = malloc(100000U);
        CHECK(owned != NULL);
        size_t at = 0U;
        at += (size_t)sprintf(owned + at, "{\"signatures\":[");
        size_t signatures = strcmp(mode, "signature-limit") == 0 ? 257U
                            : strcmp(mode, "total-limit") == 0   ? 17U
                                                                 : 1U;
        size_t parameters = strcmp(mode, "signature-limit") == 0 ? 0U
                            : strcmp(mode, "total-limit") == 0   ? 256U
                                                                 : 257U;
        for (size_t i = 0U; i < signatures; ++i)
        {
            at += (size_t)sprintf(owned + at, "%s{\"label\":\"a\",\"parameters\":[", i == 0U ? "" : ",");
            for (size_t j = 0U; j < parameters; ++j)
                at += (size_t)sprintf(owned + at, "%s{\"label\":\"a\"}", j == 0U ? "" : ",");
            at += (size_t)sprintf(owned + at, "]}");
        }
        strcpy(owned + at, "]}");
        json = owned;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (example == NULL && strcmp(mode, "cancelled") != 0 && strcmp(mode, "arguments") != 0)
        CHECK(0);
    if (legacy)
    {
        UmiLanguageRuntimeSignatureResult value;
        memset(&value, 0x55, sizeof(value));
        CHECK(umi_language_runtime_decode_signature(json, &value) == expected);
        if (expected == UMI_STATUS_OK)
        {
            CHECK(value.available && value.active_parameter == 1U);
            CHECK(strcmp(value.label, "sum(left, right)") == 0);
        }
        else
            CHECK(value.label[0] == '\0' && !value.available);
        free(owned);
        return 0;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    UmiLanguageSignatureCatalogue *catalogue = (UmiLanguageSignatureCatalogue *)(uintptr_t)1U;
    UmiStatus status =
        response ? UmiLanguageSignatureCatalogueReadResponse(
                       json, strlen(json), strcmp(mode, "wrong-id") == 0 ? 8U : 7U, cancel, &catalogue)
                 : UmiLanguageSignatureCatalogueCreate(json, strlen(json), cancel, &catalogue);
    CHECK(status == expected);
    if (owned != NULL)
    {
        memset(owned, '?', strlen(owned));
        free(owned);
    }
    if (expected != UMI_STATUS_OK)
        CHECK(catalogue == NULL);
    else
    {
        size_t count = example == NULL ? 1U : example->count;
        CHECK(UmiLanguageSignatureCatalogueCount(catalogue) == count);
        CHECK(UmiLanguageSignatureCatalogueActive(catalogue) == (count == 0U       ? SIZE_MAX
                                                                 : example == NULL ? 0U
                                                                                   : example->active));
        if (count != 0U)
        {
            UmiLanguageSignature signature;
            size_t active = UmiLanguageSignatureCatalogueActive(catalogue);
            CHECK(UmiLanguageSignatureCatalogueAt(catalogue, active, &signature) == UMI_STATUS_OK);
            int parameter = example == NULL ? 1 : example->parameter;
            CHECK(signature.active_parameter == (parameter < 0 ? SIZE_MAX : (size_t)parameter));
            if (parameter >= 0)
            {
                UmiLanguageSignatureParameter value;
                CHECK(UmiLanguageSignatureCatalogueParameter(catalogue, active, (size_t)parameter, &value) ==
                      UMI_STATUS_OK);
                CHECK(strcmp(value.label, example == NULL ? "right" : example->label) == 0);
                CHECK(value.has_label_span == (example == NULL ? 1 : example->span));
                if (value.has_label_span)
                {
                    CHECK(value.start_byte == (example == NULL ? 10U : example->first));
                    CHECK(value.end_byte == (example == NULL ? 15U : example->last));
                }
            }
            if (strcmp(mode, "valid") == 0)
            {
                UmiLanguageSignatureParameter value;
                CHECK(strcmp(signature.documentation.text, "Add two values.") == 0);
                CHECK(UmiLanguageSignatureCatalogueParameter(catalogue, 0U, 1U, &value) == UMI_STATUS_OK);
                CHECK(value.documentation.kind == UMI_LANGUAGE_SIGNATURE_MARKDOWN &&
                      strcmp(value.documentation.text, "**Second** value.") == 0);
            }
        }
        if (strcmp(mode, "arguments") == 0)
        {
            UmiLanguageSignature before = {0}, output = before;
            output.parameter_count = 777U;
            CHECK(UmiLanguageSignatureCatalogueAt(catalogue, SIZE_MAX, &output) == UMI_STATUS_NOT_FOUND &&
                  output.parameter_count == 777U);
            CHECK(UmiLanguageSignatureCatalogueAt(NULL, 0U, &output) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLanguageSignatureCatalogueCreate(json, strlen(json), NULL, NULL) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLanguageSignatureCatalogueReadResponse(json, strlen(json), 1U, NULL, NULL) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLanguageSignatureCatalogueActive(NULL) == SIZE_MAX &&
                  UmiLanguageSignatureCatalogueCount(NULL) == 0U);
        }
    }
    UmiLanguageSignatureCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
