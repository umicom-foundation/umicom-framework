/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_workspace_edit_scope.c
 * PURPOSE: Refuse partial workspace application when a host owns only one captured document.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/workspace_edit_catalogue.h"
#include <stdio.h>
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
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"single",       "other",     "multiple",  "empty-other-group", "empty",
                           "empty-edits",  "null-uri",  "empty-uri", "null-catalogue",    "null-output",
                           "cancelled",    "version",   "stale",     "missing-version",   "source-invalid",
                           "source-caret", "annotation"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    const char *json = "{\"changes\":{\"file:///"
                       "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                       "\"character\":3}},\"newText\":\"total\"}]}}";
    const char *uri = "file:///main.c", *source = "sum";
    size_t bytes = 3U, caret = 1U;
    const char *expected = "total";
    UmiStatus wanted = UMI_STATUS_OK;
    int32_t version = 1;
    const int32_t *source_version = &version;
    if (strcmp(mode, "other") == 0)
    {
        uri = "file:///other.c";
        wanted = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (strcmp(mode, "multiple") == 0 || strcmp(mode, "empty-other-group") == 0)
    {
        json = strcmp(mode, "multiple") == 0
                   ? "{\"changes\":{\"file:///main.c\":[],\"file:///"
                     "other.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                     "\"character\":3}},\"newText\":\"total\"}]}}"
                   : "{\"changes\":{\"file:///main.c\":[],\"file:///other.c\":[]}}";
        wanted = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (strcmp(mode, "empty") == 0)
    {
        json = "null";
        wanted = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "empty-edits") == 0)
    {
        json = "{\"changes\":{\"file:///main.c\":[]}}";
        expected = "sum";
    }
    if (strcmp(mode, "null-uri") == 0)
    {
        uri = NULL;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "empty-uri") == 0)
    {
        uri = "";
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "version") == 0 || strcmp(mode, "stale") == 0 || strcmp(mode, "missing-version") == 0 ||
        strcmp(mode, "annotation") == 0)
    {
        json = "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///"
               "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},"
               "\"end\":{\"line\":0,\"character\":3}},\"newText\":\"total\",\"annotationId\":\"reason\"}]}],"
               "\"changeAnnotations\":{\"reason\":{\"label\":\"Rename\",\"needsConfirmation\":true}}}";
        if (strcmp(mode, "stale") == 0)
        {
            version = 2;
            wanted = UMI_STATUS_INVALID_STATE;
        }
        if (strcmp(mode, "missing-version") == 0)
        {
            source_version = NULL;
            wanted = UMI_STATUS_INVALID_STATE;
        }
    }
    if (strcmp(mode, "source-invalid") == 0)
    {
        source = "a\xc0\x80";
        wanted = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "source-caret") == 0)
    {
        caret = 4U;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiLanguageWorkspaceEditCatalogue *catalogue = NULL;
    CHECK(UmiLanguageWorkspaceEditCatalogueCreate(json, strlen(json), NULL, &catalogue) == UMI_STATUS_OK);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        wanted = UMI_STATUS_CANCELLED;
    }
    const UmiLanguageWorkspaceEditCatalogue *input = catalogue;
    if (strcmp(mode, "null-catalogue") == 0)
    {
        input = NULL;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiLanguageTextEditPreview *preview = NULL;
    UmiStatus status;
    if (strcmp(mode, "null-output") == 0)
    {
        status = UmiLanguageWorkspaceEditCataloguePreviewSingleDocument(input, uri, source, bytes, caret,
                                                                        source_version, cancel, NULL);
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    else
        status = UmiLanguageWorkspaceEditCataloguePreviewSingleDocument(input, uri, source, bytes, caret,
                                                                        source_version, cancel, &preview);
    CHECK(status == wanted);
    if (status == UMI_STATUS_OK)
    {
        const char *text = NULL;
        size_t length = 0U, mapped = 0U;
        CHECK(UmiLanguageTextEditPreviewRead(preview, &text, &length, &mapped) == UMI_STATUS_OK);
        CHECK(length == strlen(expected) && strcmp(text, expected) == 0);
        if (strcmp(mode, "annotation") == 0)
        {
            UmiLanguageWorkspaceChangeAnnotation annotation;
            CHECK(UmiLanguageWorkspaceEditCatalogueAnnotation(catalogue, 0U, &annotation) == UMI_STATUS_OK &&
                  annotation.needs_confirmation);
        }
    }
    else
        CHECK(preview == NULL);
    UmiLanguageTextEditPreviewDestroy(preview);
    UmiLanguageWorkspaceEditCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
