/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_workspace_edit_preview.c
 * PURPOSE: Check source-specific workspace previews, exact coordinates, ordering and retained confirmation metadata.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/workspace_edit_catalogue.h"
#include "umicom/language_runtime/json_writer.h"
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
static void Edit(UmiLanguageRuntimeJsonWriter *writer, unsigned line, unsigned first, unsigned last,
                 const char *text, int annotation)
{
    char range[256];
    (void)snprintf(range, sizeof(range),
                   "{\"range\":{\"start\":{\"line\":%u,\"character\":%u},\"end\":{\"line\":%u,\"character\":%"
                   "u}},\"newText\":",
                   line, first, line, last);
    umi_language_runtime_json_writer_raw(writer, range);
    umi_language_runtime_json_writer_string(writer, text);
    if (annotation)
        umi_language_runtime_json_writer_raw(writer, ",\"annotationId\":\"review\"");
    umi_language_runtime_json_writer_raw(writer, "}");
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {
        "basic",    "version",       "version-missing", "version-mismatch", "null-version",    "annotation",
        "unicode",  "surrogate",     "multiline",       "overlap",          "ordered-inserts", "late-invalid",
        "too-many", "invalid-index", "invalid-source",  "invalid-caret",    "cancelled",       "empty-edits",
        "delete",   "source-owned",  "remote-uri"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    char *json = malloc(100000U);
    CHECK(json != NULL);
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, json, 100000U);
    int versioned = strncmp(mode, "version", 7U) == 0 || strcmp(mode, "null-version") == 0 ||
                    strcmp(mode, "annotation") == 0;
    if (versioned)
        umi_language_runtime_json_writer_raw(
            &writer, strcmp(mode, "null-version") == 0 ? "{\"documentChanges\":[{\"textDocument\":{\"uri\":"
                                                         "\"file:///main.c\",\"version\":null},\"edits\":["
                                                       : "{\"documentChanges\":[{\"textDocument\":{\"uri\":"
                                                         "\"file:///main.c\",\"version\":3},\"edits\":[");
    else
        umi_language_runtime_json_writer_raw(
            &writer, strcmp(mode, "remote-uri") == 0 ? "{\"changes\":{\"https://example.invalid/source\":["
                                                     : "{\"changes\":{\"file:///main.c\":[");
    const char *source = "sum + sum", *expected = "total + sum";
    size_t caret = 3U, mapped = 5U;
    UmiStatus wanted = UMI_STATUS_OK;
    size_t edit_count = 1U;
    if (strcmp(mode, "unicode") == 0 || strcmp(mode, "surrogate") == 0)
    {
        source = "a\xf0\x9f\x98\x80"
                 "x";
        expected = "azx";
        caret = 5U;
        mapped = 2U;
        Edit(&writer, 0U, 1U, strcmp(mode, "surrogate") == 0 ? 2U : 3U, "z", 0);
        if (strcmp(mode, "surrogate") == 0)
            wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "multiline") == 0)
    {
        source = "sum\r\nsum";
        expected = "sum\r\ntotal";
        caret = 5U;
        mapped = 10U;
        Edit(&writer, 1U, 0U, 3U, "total", 0);
    }
    else if (strcmp(mode, "ordered-inserts") == 0)
    {
        source = "sum";
        expected = "ABsum";
        caret = 0U;
        mapped = 2U;
        edit_count = 2U;
        Edit(&writer, 0U, 0U, 0U, "A", 0);
        umi_language_runtime_json_writer_raw(&writer, ",");
        Edit(&writer, 0U, 0U, 0U, "B", 0);
    }
    else if (strcmp(mode, "overlap") == 0 || strcmp(mode, "late-invalid") == 0)
    {
        Edit(&writer, 0U, 0U, 3U, "total", 0);
        umi_language_runtime_json_writer_raw(&writer, ",");
        Edit(&writer, strcmp(mode, "late-invalid") == 0 ? 99U : 0U, 1U, 2U, "x", 0);
        wanted = strcmp(mode, "late-invalid") == 0 ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_INVALID_STATE;
    }
    else if (strcmp(mode, "too-many") == 0)
    {
        for (size_t i = 0U; i < 257U; ++i)
        {
            if (i != 0U)
                umi_language_runtime_json_writer_raw(&writer, ",");
            Edit(&writer, 0U, 0U, 0U, "x", 0);
        }
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(mode, "empty-edits") == 0)
    {
        expected = source;
        mapped = caret;
        edit_count = 0U;
    }
    else if (strcmp(mode, "delete") == 0)
    {
        expected = " + sum";
        mapped = 0U;
        Edit(&writer, 0U, 0U, 3U, "", 0);
    }
    else
        Edit(&writer, 0U, 0U, 3U, "total", strcmp(mode, "annotation") == 0);
    umi_language_runtime_json_writer_raw(&writer, versioned ? "]}]" : "]}");
    if (strcmp(mode, "annotation") == 0)
        umi_language_runtime_json_writer_raw(
            &writer, ",\"changeAnnotations\":{\"review\":{\"label\":\"Rename\",\"needsConfirmation\":true}}");
    umi_language_runtime_json_writer_raw(&writer, "}");
    CHECK(writer.status == UMI_STATUS_OK);
    UmiLanguageWorkspaceEditCatalogue *catalogue = NULL;
    CHECK(UmiLanguageWorkspaceEditCatalogueCreate(json, writer.length, NULL, &catalogue) == UMI_STATUS_OK);
    free(json);
    json = NULL;
    int32_t revision = strcmp(mode, "version-mismatch") == 0 ? 4 : 3;
    const int32_t *identity = &revision;
    if (strcmp(mode, "version-missing") == 0)
        identity = NULL;
    if (strcmp(mode, "version-mismatch") == 0 || strcmp(mode, "version-missing") == 0)
        wanted = UMI_STATUS_INVALID_STATE;
    size_t index = strcmp(mode, "invalid-index") == 0 ? SIZE_MAX : 0U;
    if (index == SIZE_MAX)
        wanted = UMI_STATUS_NOT_FOUND;
    if (strcmp(mode, "invalid-source") == 0)
    {
        source = "a\xc0\x80";
        wanted = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "invalid-caret") == 0)
    {
        caret = SIZE_MAX;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        wanted = UMI_STATUS_CANCELLED;
    }
    char *owned = malloc(strlen(source) + 1U);
    CHECK(owned != NULL);
    strcpy(owned, source);
    UmiLanguageTextEditPreview *preview = (UmiLanguageTextEditPreview *)(uintptr_t)1U;
    CHECK(UmiLanguageWorkspaceEditCataloguePreview(catalogue, index, owned, strlen(owned), caret, identity,
                                                   cancel, &preview) == wanted);
    CHECK(strcmp(owned, source) == 0);
    if (strcmp(mode, "annotation") == 0)
    {
        UmiLanguageWorkspaceChangeAnnotation annotation;
        CHECK(UmiLanguageWorkspaceEditCatalogueAnnotation(catalogue, 0U, &annotation) == UMI_STATUS_OK);
        CHECK(annotation.needs_confirmation && strcmp(annotation.label, "Rename") == 0);
    }
    memset(owned, '?', strlen(owned));
    free(owned);
    UmiLanguageWorkspaceEditCatalogueDestroy(catalogue);
    if (wanted != UMI_STATUS_OK)
        CHECK(preview == NULL);
    else
    {
        const char *text = NULL;
        size_t bytes = 0U, position = 0U;
        CHECK(UmiLanguageTextEditPreviewRead(preview, &text, &bytes, &position) == UMI_STATUS_OK);
        CHECK(bytes == strlen(expected) && strcmp(text, expected) == 0 && position == mapped);
        CHECK(UmiLanguageTextEditPreviewCount(preview) == edit_count);
    }
    UmiLanguageTextEditPreviewDestroy(preview);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
