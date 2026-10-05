/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_completion_catalogue.c
 * PURPOSE: Exercise completion ownership, bounded metadata and failed-result publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/completion_catalogue.h"
#include <stdio.h>
#include <stdlib.h>
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

/* Each row is an independent protocol input. A refused response must not leave
 * a partially populated catalogue that the editor could accidentally present. */
typedef struct CatalogueCase
{
    const char *name, *json;
    UmiStatus expected;
} CatalogueCase;
static const CatalogueCase cases[] = {
    {"array",
     "[{\"label\":\"puts\",\"kind\":3,\"detail\":\"write\",\"sortText\":\"a\",\"filterText\":\"put\"}]",
     UMI_STATUS_OK},
    {"list", "{\"isIncomplete\":true,\"items\":[{\"label\":\"puts\"}]}", UMI_STATUS_OK},
    {"null", "null", UMI_STATUS_OK},
    {"empty", "[]", UMI_STATUS_OK},
    {"invalid", "42", UMI_STATUS_PARSE_ERROR},
    {"missing-incomplete", "{\"items\":[]}", UMI_STATUS_PARSE_ERROR},
    {"missing-items", "{\"isIncomplete\":false}", UMI_STATUS_PARSE_ERROR},
    {"label-type", "[{\"label\":12}]", UMI_STATUS_PARSE_ERROR},
    {"missing-label", "[{}]", UMI_STATUS_PARSE_ERROR},
    {"detail-type", "[{\"label\":\"x\",\"detail\":false}]", UMI_STATUS_PARSE_ERROR},
    {"duplicate", "[{\"label\":\"x\",\"label\":\"y\"}]", UMI_STATUS_ALREADY_EXISTS},
    {"duplicate-list", "{\"isIncomplete\":false,\"items\":[],\"items\":[]}", UMI_STATUS_ALREADY_EXISTS},
    {"format", "[{\"label\":\"x\",\"insertTextFormat\":3}]", UMI_STATUS_PARSE_ERROR},
    {"defaults",
     "{\"isIncomplete\":false,\"itemDefaults\":{\"insertTextFormat\":2,\"insertTextMode\":2},\"items\":[{"
     "\"label\":\"x\"}]}",
     UMI_STATUS_OK},
    {"override",
     "{\"isIncomplete\":false,\"itemDefaults\":{\"insertTextFormat\":2},\"items\":[{\"label\":\"x\","
     "\"insertTextFormat\":1}]}",
     UMI_STATUS_OK},
    {"defaults-type", "{\"isIncomplete\":false,\"itemDefaults\":null,\"items\":[]}", UMI_STATUS_PARSE_ERROR},
    {"merge-mode", "{\"isIncomplete\":false,\"applyKind\":{},\"items\":[]}", UMI_STATUS_NOT_IMPLEMENTED},
    {"additional-type", "[{\"label\":\"x\",\"additionalTextEdits\":{}}]", UMI_STATUS_PARSE_ERROR},
    {"unicode", "[{\"label\":\"caf\\u00e9\",\"detail\":\"\\ud83d\\ude00\"}]", UMI_STATUS_OK},
    {"malformed-unicode", "[{\"label\":\"\\ud800\"}]", UMI_STATUS_PARSE_ERROR}};
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    UmiLanguageCompletionCatalogue *catalogue = NULL;
    if (strcmp(name, "owned") == 0)
    {
        char json[] = "[{\"label\":\"original\"}]";
        CHECK(UmiLanguageCompletionCatalogueCreate(json, strlen(json), NULL, &catalogue) == UMI_STATUS_OK);
        memset(json, '?', sizeof(json) - 1U);
        UmiLanguageCompletionChoice choice;
        CHECK(UmiLanguageCompletionCatalogueAt(catalogue, 0U, &choice) == UMI_STATUS_OK);
        CHECK(strcmp(choice.label, "original") == 0);
    }
    else if (strcmp(name, "at-atomic") == 0)
    {
        CHECK(UmiLanguageCompletionCatalogueCreate("[]", 2U, NULL, &catalogue) == UMI_STATUS_OK);
        UmiLanguageCompletionChoice choice, before;
        memset(&choice, 0x5a, sizeof(choice));
        memcpy(&before, &choice, sizeof(choice));
        CHECK(UmiLanguageCompletionCatalogueAt(catalogue, 0U, &choice) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&choice, &before, sizeof(choice)) == 0);
    }
    else if (strcmp(name, "cancel") == 0)
    {
        UmiCancellationToken *cancel = NULL;
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        CHECK(UmiLanguageCompletionCatalogueCreate("[]", 2U, cancel, &catalogue) == UMI_STATUS_CANCELLED);
        CHECK(catalogue == NULL);
        umi_cancellation_token_destroy(cancel);
    }
    else if (strcmp(name, "item-limit") == 0 || strcmp(name, "item-boundary") == 0)
    {
        size_t count = strcmp(name, "item-boundary") == 0 ? 2048U : 2049U;
        char *json = malloc(count * 14U + 3U);
        CHECK(json != NULL);
        size_t at = 0U;
        json[at++] = '[';
        for (size_t i = 0U; i < count; ++i)
        {
            if (i != 0U)
                json[at++] = ',';
            memcpy(json + at, "{\"label\":\"x\"}", 13U);
            at += 13U;
        }
        json[at++] = ']';
        json[at] = '\0';
        UmiStatus status = UmiLanguageCompletionCatalogueCreate(json, at, NULL, &catalogue);
        CHECK(status == (count == 2048U ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(UmiLanguageCompletionCatalogueCount(catalogue) == (count == 2048U ? count : 0U));
        free(json);
    }
    else if (strcmp(name, "byte-limit") == 0)
    {
        size_t size = 1024U * 1024U + 1U;
        char *json = malloc(size);
        CHECK(json != NULL);
        memset(json, ' ', size);
        json[0] = '[';
        json[1] = ']';
        CHECK(UmiLanguageCompletionCatalogueCreate(json, size, NULL, &catalogue) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(catalogue == NULL);
        free(json);
    }
    else if (strcmp(name, "label-limit") == 0)
    {
        char json[300];
        memcpy(json, "[{\"label\":\"", 11U);
        memset(json + 11U, 'x', 256U);
        memcpy(json + 267U, "\"}]", 4U);
        CHECK(UmiLanguageCompletionCatalogueCreate(json, strlen(json), NULL, &catalogue) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(catalogue == NULL);
    }
    else if (strcmp(name, "arguments") == 0)
    {
        CHECK(UmiLanguageCompletionCatalogueCreate(NULL, 1U, NULL, &catalogue) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(catalogue == NULL);
        CHECK(UmiLanguageCompletionCatalogueCreate("[]", 2U, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLanguageCompletionCatalogueCount(NULL) == 0U);
        CHECK(!UmiLanguageCompletionCatalogueIncomplete(NULL));
    }
    else
    {
        size_t i;
        for (i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
            if (strcmp(name, cases[i].name) == 0)
                break;
        CHECK(i < sizeof(cases) / sizeof(cases[0]));
        CHECK(UmiLanguageCompletionCatalogueCreate(cases[i].json, strlen(cases[i].json), NULL, &catalogue) ==
              cases[i].expected);
        if (cases[i].expected != UMI_STATUS_OK)
            CHECK(catalogue == NULL);
        else if (UmiLanguageCompletionCatalogueCount(catalogue) > 0U)
        {
            UmiLanguageCompletionChoice choice;
            CHECK(UmiLanguageCompletionCatalogueAt(catalogue, 0U, &choice) == UMI_STATUS_OK);
            CHECK(UmiLanguageCompletionCatalogueCount(catalogue) == 1U);
            if (strcmp(name, "array") == 0)
            {
                CHECK(strcmp(choice.label, "puts") == 0 && choice.kind == 3U);
                CHECK(strcmp(choice.detail, "write") == 0 && strcmp(choice.sort_text, "a") == 0);
                CHECK(strcmp(choice.filter_text, "put") == 0);
            }
            if (strcmp(name, "list") == 0)
                CHECK(UmiLanguageCompletionCatalogueIncomplete(catalogue));
            if (strcmp(name, "defaults") == 0)
                CHECK(choice.insert_text_format == 2U && choice.insert_text_mode == 2U);
            if (strcmp(name, "override") == 0)
                CHECK(choice.insert_text_format == 1U);
            if (strcmp(name, "unicode") == 0)
                CHECK(strcmp(choice.label, "caf\xc3\xa9") == 0);
        }
        else
            CHECK(strcmp(name, "null") == 0 || strcmp(name, "empty") == 0);
    }
    UmiLanguageCompletionCatalogueDestroy(catalogue);
    return 0;
}
