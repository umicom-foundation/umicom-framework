/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_workspace_edit_catalogue.c
 * PURPOSE: Check complete workspace edit groups, revision requirements, annotations and all-or-nothing publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/workspace_edit_catalogue.h"
#include "umicom/language_runtime/decoders/workspace_edit.h"
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
    size_t documents, annotations;
    int has_version;
    int32_t version;
    size_t edits;
    int annotation;
} Example;
static const Example examples[] = {
    {"changes",
     "{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}}",
     UMI_STATUS_OK, 1U, 0U, 0, 0, 1U, -1},
    {"null", "null", UMI_STATUS_OK, 0U, 0U, 0, 0, 1U, -1},
    {"empty", "{}", UMI_STATUS_OK, 0U, 0U, 0, 0, 1U, -1},
    {"empty-map", "{\"changes\":{}}", UMI_STATUS_OK, 0U, 0U, 0, 0, 1U, -1},
    {"empty-array", "{\"documentChanges\":[]}", UMI_STATUS_OK, 0U, 0U, 0, 0, 1U, -1},
    {"empty-edits", "{\"changes\":{\"file:///main.c\":[]}}", UMI_STATUS_OK, 1U, 0U, 0, 0, 0U, -1},
    {"version",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\"}]}]}",
     UMI_STATUS_OK, 1U, 0U, 1, 1, 1U, -1},
    {"zero-version",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":0},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\"}]}]}",
     UMI_STATUS_OK, 1U, 0U, 1, 0, 1U, -1},
    {"negative-version",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":-1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\"}]}]}",
     UMI_STATUS_OK, 1U, 0U, 1, -1, 1U, -1},
    {"minimum-version",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":-2147483648},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},"
     "\"end\":{\"line\":0,\"character\":3}},\"newText\":\"total\"}]}]}",
     UMI_STATUS_OK, 1U, 0U, 1, -2147483648, 1U, -1},
    {"null-version",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":null},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\"}]}]}",
     UMI_STATUS_OK, 1U, 0U, 0, 0, 1U, -1},
    {"prefer-document-changes",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\"}]}],\"changes\":{\"file:///ignored.c\":[]}}",
     UMI_STATUS_OK, 1U, 0U, 1, 1, 1U, -1},
    {"annotation",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\",\"annotationId\":\"review\"}]}],"
     "\"changeAnnotations\":{\"review\":{\"label\":\"Rename references\",\"description\":\"Includes "
     "comments.\",\"needsConfirmation\":true}}}",
     UMI_STATUS_OK, 1U, 1U, 1, 1, 1U, 0},
    {"annotation-no-confirmation",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\",\"annotationId\":\"review\"}]}],"
     "\"changeAnnotations\":{\"review\":{\"label\":\"Rename references\",\"description\":\"Includes "
     "comments.\",\"needsConfirmation\":false}}}",
     UMI_STATUS_OK, 1U, 1U, 1, 1, 1U, 0},
    {"unused-annotation",
     "{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]},\"changeAnnotations\":{\"unused\":{\"label\":\"Information\"}}}",
     UMI_STATUS_OK, 1U, 1U, 0, 0, 1U, -1},
    {"missing-annotation",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\",\"annotationId\":\"review\"}]}]}",
     UMI_STATUS_NOT_FOUND, 1U, 0U, 0, 0, 1U, -1},
    {"confirmation-type",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\",\"annotationId\":\"review\"}]}],"
     "\"changeAnnotations\":{\"review\":{\"label\":\"Rename references\",\"description\":\"Includes "
     "comments.\",\"needsConfirmation\":\"yes\"}}}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"annotation-label-missing",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\",\"annotationId\":\"review\"}]}],"
     "\"changeAnnotations\":{\"review\":{\"description\":\"Includes "
     "comments.\",\"needsConfirmation\":true}}}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"annotation-description-type",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\",\"annotationId\":\"review\"}]}],"
     "\"changeAnnotations\":{\"review\":{\"label\":\"Rename "
     "references\",\"description\":4,\"needsConfirmation\":true}}}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"map-annotation",
     "{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\",\"annotationId\":\"review\"}]}}",
     UMI_STATUS_NOT_IMPLEMENTED, 1U, 0U, 0, 0, 1U, -1},
    {"version-type",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":\"one\"},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\"}]}]}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"version-fraction",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":1.5},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\"}]}]}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"version-overflow",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":2147483648},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":"
     "{\"line\":0,\"character\":3}},\"newText\":\"total\"}]}]}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"version-missing",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\"},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
     "\"character\":3}},\"newText\":\"total\"}]}]}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"repeated-document",
     "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\"}]},{\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\"}]}]}",
     UMI_STATUS_ALREADY_EXISTS, 1U, 0U, 0, 0, 1U, -1},
    {"resource-create", "{\"documentChanges\":[{\"kind\":\"create\",\"uri\":\"file:///main.c\"}]}",
     UMI_STATUS_NOT_IMPLEMENTED, 1U, 0U, 0, 0, 1U, -1},
    {"resource-rename", "{\"documentChanges\":[{\"kind\":\"rename\",\"uri\":\"file:///main.c\"}]}",
     UMI_STATUS_NOT_IMPLEMENTED, 1U, 0U, 0, 0, 1U, -1},
    {"resource-delete", "{\"documentChanges\":[{\"kind\":\"delete\",\"uri\":\"file:///main.c\"}]}",
     UMI_STATUS_NOT_IMPLEMENTED, 1U, 0U, 0, 0, 1U, -1},
    {"changes-type", "{\"changes\":[]}", UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"documents-type", "{\"documentChanges\":{}}", UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"annotations-type", "{\"changeAnnotations\":[]}", UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"edits-type", "{\"changes\":{\"file:///main.c\":{}}}", UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"bad-uri",
     "{\"changes\":{\"relative.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
     "\"character\":3}},\"newText\":\"total\"}]}}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"text-type",
     "{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":4}]}}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"negative-range",
     "{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":-1}},"
     "\"newText\":\"total\"}]}}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"reversed-range",
     "{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":4},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}}",
     UMI_STATUS_PARSE_ERROR, 1U, 0U, 0, 0, 1U, -1},
    {"unknown-edit-shape",
     "{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\",\"snippet\":true}]}}",
     UMI_STATUS_NOT_IMPLEMENTED, 1U, 0U, 0, 0, 1U, -1},
    {"two-documents",
     "{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}],\"file:///other.c\":[]}}",
     UMI_STATUS_OK, 2U, 0U, 0, 0, 1U, -1},
    {"duplicate-uri", "{\"changes\":{\"file:///main.c\":[],\"file:///main.c\":[]}}",
     UMI_STATUS_ALREADY_EXISTS, 0U, 0U, 0, 0, 0U, -1},
    {"duplicate-member", "{\"changes\":{},\"changes\":{}}", UMI_STATUS_ALREADY_EXISTS, 0U, 0U, 0, 0, 0U, -1},
    {"duplicate-annotation",
     "{\"changeAnnotations\":{\"same\":{\"label\":\"one\"},\"same\":{\"label\":\"two\"}}}",
     UMI_STATUS_ALREADY_EXISTS, 0U, 0U, 0, 0, 0U, -1},
};
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const Example *example = NULL;
    for (size_t i = 0U; i < sizeof(examples) / sizeof(examples[0]); ++i)
        if (strcmp(mode, examples[i].name) == 0)
            example = &examples[i];
    const char *extra[] = {"owned",          "response",       "wrong-id",          "error",
                           "legacy-changes", "legacy-version", "legacy-annotation", "legacy-partial",
                           "document-limit", "edit-limit",     "annotation-limit",  "cancelled",
                           "arguments"};
    int known = example != NULL;
    for (size_t i = 0U; i < sizeof(extra) / sizeof(extra[0]); ++i)
        if (strcmp(mode, extra[i]) == 0)
            known = 1;
    CHECK(known);
    const char *json = example == NULL ? examples[0].json : example->json;
    UmiStatus expected = example == NULL ? UMI_STATUS_OK : example->status;
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
             strncmp(mode, "legacy-", 7U) == 0)
    {
        if (strcmp(mode, "legacy-version") == 0)
            json = examples[6].json;
        if (strcmp(mode, "legacy-annotation") == 0)
            json = examples[12].json;
        if (strcmp(mode, "legacy-partial") == 0)
        {
            json = "{\"changes\":{\"file:///"
                   "first.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                   "\"character\":1}},\"newText\":\"a\"}],\"file:///"
                   "second.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                   "\"character\":1}},\"newText\":4}]}}";
            expected = UMI_STATUS_PARSE_ERROR;
        }
        owned = malloc(strlen(json) + 150U);
        CHECK(owned != NULL);
        if (strcmp(mode, "error") == 0)
        {
            strcpy(owned,
                   "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":-32603,\"message\":\"failure\"}}");
            expected = UMI_STATUS_UNAVAILABLE;
        }
        else
            (void)sprintf(owned, "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":%s}", json);
        json = owned;
        response = 1;
        legacy = strncmp(mode, "legacy-", 7U) == 0;
        if (strcmp(mode, "wrong-id") == 0)
            expected = UMI_STATUS_NOT_FOUND;
        if (strcmp(mode, "legacy-version") == 0 || strcmp(mode, "legacy-annotation") == 0)
            expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    else if (strcmp(mode, "document-limit") == 0 || strcmp(mode, "edit-limit") == 0 ||
             strcmp(mode, "annotation-limit") == 0)
    {
        owned = malloc(200000U);
        CHECK(owned != NULL);
        size_t at = 0U;
        if (strcmp(mode, "document-limit") == 0)
        {
            at += (size_t)sprintf(owned + at, "{\"changes\":{");
            for (size_t i = 0U; i < 257U; ++i)
                at += (size_t)sprintf(owned + at, "%s\"file:///file%zu.c\":[]", i == 0U ? "" : ",", i);
            strcpy(owned + at, "}}");
        }
        else if (strcmp(mode, "annotation-limit") == 0)
        {
            at += (size_t)sprintf(owned + at, "{\"changeAnnotations\":{");
            for (size_t i = 0U; i < 257U; ++i)
                at += (size_t)sprintf(owned + at, "%s\"annotation%zu\":{\"label\":\"Review\"}",
                                      i == 0U ? "" : ",", i);
            strcpy(owned + at, "}}");
        }
        else
        {
            at += (size_t)sprintf(owned + at, "{\"changes\":{\"file:///main.c\":[");
            for (size_t i = 0U; i < 4097U; ++i)
                at += (size_t)sprintf(owned + at, "%s{}", i == 0U ? "" : ",");
            strcpy(owned + at, "]}}");
        }
        json = owned;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (example == NULL && strcmp(mode, "cancelled") != 0 && strcmp(mode, "arguments") != 0)
        CHECK(0);
    if (legacy)
    {
        UmiLanguageRuntimeWorkspaceEdit *output = malloc(sizeof(*output));
        CHECK(output != NULL);
        memset(output, 0x55, sizeof(*output));
        CHECK(umi_language_runtime_decode_workspace_edit(json, output) == expected);
        if (expected == UMI_STATUS_OK)
            CHECK(output->count == 1U && strcmp(output->items[0].edit.new_text, "total") == 0);
        else
            CHECK(output->count == 0U && output->items[0].uri[0] == '\0');
        free(output);
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
    UmiLanguageWorkspaceEditCatalogue *catalogue = (UmiLanguageWorkspaceEditCatalogue *)(uintptr_t)1U;
    UmiStatus status =
        response ? UmiLanguageWorkspaceEditCatalogueReadResponse(
                       json, strlen(json), strcmp(mode, "wrong-id") == 0 ? 8U : 7U, cancel, &catalogue)
                 : UmiLanguageWorkspaceEditCatalogueCreate(json, strlen(json), cancel, &catalogue);
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
        size_t count = example == NULL ? 1U : example->documents;
        CHECK(UmiLanguageWorkspaceEditCatalogueCount(catalogue) == count);
        CHECK(UmiLanguageWorkspaceEditCatalogueAnnotationCount(catalogue) ==
              (example == NULL ? 0U : example->annotations));
        if (count != 0U)
        {
            UmiLanguageWorkspaceDocumentChange document;
            CHECK(UmiLanguageWorkspaceEditCatalogueDocument(catalogue, 0U, &document) == UMI_STATUS_OK);
            CHECK(strcmp(document.uri, "file:///main.c") == 0 &&
                  document.edit_count == (example == NULL ? 1U : example->edits));
            CHECK(document.has_version == (example == NULL ? 0 : example->has_version));
            if (document.has_version)
                CHECK(document.version == example->version);
            if (document.edit_count != 0U)
            {
                UmiLanguageWorkspaceTextChange edit;
                CHECK(UmiLanguageWorkspaceEditCatalogueEdit(catalogue, 0U, 0U, &edit) == UMI_STATUS_OK);
                CHECK(strcmp(edit.text, "total") == 0 && edit.text_bytes == 5U &&
                      edit.range.end.utf16_column == 3U);
                CHECK(edit.annotation ==
                      (example == NULL || example->annotation < 0 ? SIZE_MAX : (size_t)example->annotation));
                if (edit.annotation != SIZE_MAX)
                {
                    UmiLanguageWorkspaceChangeAnnotation annotation;
                    CHECK(UmiLanguageWorkspaceEditCatalogueAnnotation(catalogue, edit.annotation,
                                                                      &annotation) == UMI_STATUS_OK);
                    CHECK(strcmp(annotation.label, "Rename references") == 0 &&
                          strcmp(annotation.description, "Includes comments.") == 0);
                    CHECK(annotation.needs_confirmation == (strcmp(mode, "annotation") == 0));
                }
            }
        }
        if (strcmp(mode, "arguments") == 0)
        {
            UmiLanguageWorkspaceDocumentChange unchanged = {0};
            unchanged.edit_count = 77U;
            CHECK(UmiLanguageWorkspaceEditCatalogueDocument(catalogue, SIZE_MAX, &unchanged) ==
                      UMI_STATUS_NOT_FOUND &&
                  unchanged.edit_count == 77U);
            CHECK(UmiLanguageWorkspaceEditCatalogueDocument(NULL, 0U, &unchanged) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLanguageWorkspaceEditCatalogueCreate(json, strlen(json), NULL, NULL) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLanguageWorkspaceEditCatalogueCount(NULL) == 0U &&
                  UmiLanguageWorkspaceEditCatalogueAnnotationCount(NULL) == 0U);
        }
    }
    UmiLanguageWorkspaceEditCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
