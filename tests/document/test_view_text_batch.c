/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_view_text_batch.c
 * PURPOSE: Check private view transaction reservation, atomic publication and abort semantics.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../src/ui/document_text_batch_internal.h"
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
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"publish",     "abort",      "large",          "unchanged",      "duplicate",
                           "stale-model", "stale-text", "wrong-identity", "wrong-uri",      "wrong-bytes",
                           "readonly",    "missing",    "invalid-field",  "invalid-cursor", "embedded-zero",
                           "too-large",   "arguments"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    UmiUiDocumentViewModel *model = NULL;
    CHECK(umi_ui_document_view_model_create(&model) == UMI_STATUS_OK);
    UmiUiDocumentViewSnapshot *after = calloc(2U, sizeof(*after));
    CHECK(after != NULL);
    UmiUiDocumentTextInfo info[2];
    UmiUiDocumentTextBatchItem items[2];
    char *large = malloc(65537U);
    CHECK(large != NULL);
    memset(large, 'x', 65536U);
    large[65536] = '\0';
    const char *new_text = strcmp(mode, "large") == 0       ? large
                           : strcmp(mode, "unchanged") == 0 ? "old"
                                                            : "new";
    for (size_t i = 0U; i < 2U; ++i)
    {
        strcpy(after[i].view_id, i == 0U ? "view.first" : "view.second");
        strcpy(after[i].document_id, i == 0U ? "doc.first" : "doc.second");
        strcpy(after[i].title, "Keep title");
        after[i].active = i == 0U;
        after[i].pinned = 1;
        CHECK(UmiUiDocumentViewModelUpsertText(model, &after[i], "old", 3U) == UMI_STATUS_OK);
        CHECK(UmiUiDocumentViewModelTextInfo(model, after[i].view_id, &info[i]) == UMI_STATUS_OK);
        after[i].dirty = 1;
        items[i] = (UmiUiDocumentTextBatchItem){&after[i], info[i].text_revision, "old", 3U,
                                                new_text,  strlen(new_text)};
    }
    uint64_t revision = umi_ui_document_view_model_revision(model);
    UmiStatus wanted = UMI_STATUS_OK;
    if (strcmp(mode, "duplicate") == 0)
    {
        items[1] = items[0];
        wanted = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(mode, "stale-model") == 0)
    {
        --revision;
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "stale-text") == 0)
    {
        ++items[1].expected_text_revision;
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "wrong-identity") == 0)
    {
        strcpy(after[1].document_id, "different");
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "wrong-uri") == 0)
    {
        strcpy(after[1].uri, "different");
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "wrong-bytes") == 0)
    {
        items[1].before_text = "bad";
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "readonly") == 0)
    {
        after[1].read_only = 1;
        wanted = UMI_STATUS_PERMISSION_DENIED;
    }
    if (strcmp(mode, "missing") == 0)
    {
        strcpy(after[1].view_id, "missing");
        wanted = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "invalid-field") == 0)
    {
        memset(after[1].title, 'x', sizeof(after[1].title));
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-cursor") == 0)
    {
        after[1].cursor_offset = 100U;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "embedded-zero") == 0)
    {
        items[1].after_text = "a\0b";
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "too-large") == 0)
    {
        items[1].after_bytes = UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    uint64_t before_revision = umi_ui_document_view_model_revision(model);
    UmiUiDocumentTextBatch *guard = (UmiUiDocumentTextBatch *)1;
    CHECK(UmiUiDocumentTextBatchAcquire(model, revision, items, 2U, &guard) == wanted);
    int published = wanted == UMI_STATUS_OK && strcmp(mode, "abort") != 0;
    if (published)
        UmiUiDocumentTextBatchPublish(guard);
    else
        UmiUiDocumentTextBatchAbort(guard);
    for (size_t i = 0U; i < 2U; ++i)
    {
        const char *id = i == 0U ? "view.first" : "view.second";
        char *text = NULL;
        size_t bytes = 0U;
        UmiUiDocumentTextInfo current;
        CHECK(UmiUiDocumentViewModelCopyText(model, id, &text, &bytes) == UMI_STATUS_OK);
        CHECK(strcmp(text, published ? new_text : "old") == 0);
        UmiUiDocumentViewModelFreeText(text);
        CHECK(UmiUiDocumentViewModelTextInfo(model, id, &current) == UMI_STATUS_OK);
        CHECK(current.text_revision ==
              info[i].text_revision + (published && strcmp(mode, "unchanged") != 0 ? 1U : 0U));
        UmiUiDocumentViewSnapshot view;
        CHECK(umi_ui_document_view_model_find(model, id, &view) == UMI_STATUS_OK);
        CHECK(strcmp(view.title, "Keep title") == 0 && view.pinned && view.active == (i == 0U));
    }
    CHECK(umi_ui_document_view_model_revision(model) == before_revision + (published ? 1U : 0U));
    if (strcmp(mode, "arguments") == 0)
    {
        CHECK(UmiUiDocumentTextBatchAcquire(NULL, revision, items, 2U, &guard) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              guard == NULL);
        CHECK(UmiUiDocumentTextBatchAcquire(model, revision, NULL, 2U, &guard) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiUiDocumentTextBatchAcquire(model, revision, items, 0U, &guard) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiUiDocumentTextBatchAcquire(model, revision, items, UMI_UI_DOCUMENT_VIEW_MAX + 1U, &guard) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        UmiUiDocumentTextBatchAbort(NULL);
        UmiUiDocumentTextBatchPublish(NULL);
    }
    free(after);
    free(large);
    umi_ui_document_view_model_destroy(model);
    return 0;
}
