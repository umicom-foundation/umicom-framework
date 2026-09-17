/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_document_content.c
 *
 * PURPOSE:
 *   Exercise complete document drafts independently of their bounded previews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/document_view.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

/* Use ordinary tab metadata without constructing a graphical application. */
static UmiUiDocumentViewSnapshot View(const char *id)
{
    UmiUiDocumentViewSnapshot view = {0};
    snprintf(view.view_id, sizeof view.view_id, "%s", id);
    snprintf(view.document_id, sizeof view.document_id, "document.%s", id);
    snprintf(view.title, sizeof view.title, "%s.c", id);
    strcpy(view.language_id, "c");
    view.closable = 1;
    return view;
}

/* Copying a complete draft must not expose the model's mutable allocation. */
static int Equals(UmiUiDocumentViewModel *model, const char *id, const char *text, size_t expected)
{
    char *copy = NULL;
    size_t length = 0U;
    CHECK(UmiUiDocumentViewModelCopyText(model, id, &copy, &length) == UMI_STATUS_OK);
    int equal = length == expected && copy[length] == '\0' && memcmp(copy, text, length) == 0;
    UmiUiDocumentViewModelFreeText(copy);
    CHECK(equal);
    return 0;
}

typedef struct ConcurrentRead {
    UmiUiDocumentViewModel *model;
    atomic_int finished;
} ConcurrentRead;

/* A reader sees either one complete publication or the next, never a mixture. */
static int ReadRepeatedly(void *context)
{
    ConcurrentRead *read = context;
    do {
        char *copy = NULL;
        size_t length = 0U;
        if (UmiUiDocumentViewModelCopyText(read->model, "notes", &copy, &length) != UMI_STATUS_OK) return 1;
        int valid = length == 32768U && (copy[0] == 'a' || copy[0] == 'b');
        for (size_t n = 1U; valid && n < length; ++n) valid = copy[n] == copy[0];
        UmiUiDocumentViewModelFreeText(copy);
        if (!valid) return 1;
    } while (!atomic_load(&read->finished));
    return 0;
}

/* Each named case starts with an empty model and independent owned buffers. */
static int Run(UmiUiDocumentViewModel *model, char *large, const char *name)
{
    const size_t length = 131072U;
    UmiUiDocumentViewSnapshot view = View("notes"), after;
    UmiUiDocumentTextInfo beforeInfo, info;
    memset(large, 'x', UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES);
    large[UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES] = '\0';
    if (strcmp(name, "maximum") == 0) {
        CHECK(UmiUiDocumentViewModelUpsertText(model, &view, large, UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES) == UMI_STATUS_OK);
        CHECK(Equals(model, view.view_id, large, UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES) == 0);
        uint64_t revision = umi_ui_document_view_model_revision(model);
        CHECK(UmiUiDocumentViewModelUpsertText(model, &view, large, UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_ui_document_view_model_revision(model) == revision);
        return 0;
    }
    if (strcmp(name, "budget-release") == 0) {
        unsigned count = 0U;
        UmiStatus status;
        for (; count < 20U; ++count) {
            char id[32]; snprintf(id, sizeof id, "file%u", count); view = View(id);
            status = UmiUiDocumentViewModelUpsertText(model, &view, large, UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES);
            if (status == UMI_STATUS_CAPACITY_EXCEEDED) break;
            CHECK(status == UMI_STATUS_OK);
        }
        CHECK(count > 0U && count < 20U);
        CHECK(umi_ui_document_view_model_count(model) == count);
        CHECK(umi_ui_document_view_model_remove(model, "file0") == UMI_STATUS_OK);
        CHECK(UmiUiDocumentViewModelUpsertText(model, &view, large, UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES) == UMI_STATUS_OK);
        CHECK(Equals(model, view.view_id, large, UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES) == 0);
        return 0;
    }
    if (strcmp(name, "utf8-preview") == 0) {
        size_t boundary = UMI_UI_DOCUMENT_CONTENT_CAPACITY - 2U;
        memcpy(large + boundary, "\xe2\x82\xac", 3U);
        CHECK(UmiUiDocumentViewModelUpsertText(model, &view, large, length) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(model, view.view_id, &after) == UMI_STATUS_OK);
        CHECK(strlen(after.source_text) == boundary);
        CHECK(Equals(model, view.view_id, large, length) == 0);
        return 0;
    }
    if (strcmp(name, "concurrent-copies") == 0) {
        memset(large, 'a', 32768U);
        CHECK(UmiUiDocumentViewModelUpsertText(model, &view, large, 32768U) == UMI_STATUS_OK);
        ConcurrentRead read = {.model = model};
        atomic_init(&read.finished, 0);
        UmiThread *thread = NULL;
        CHECK(umi_thread_start(ReadRepeatedly, &read, &thread) == UMI_STATUS_OK);
        int publishFailed = 0, result = -1;
        for (unsigned n = 0U; n < 200U; ++n) {
            memset(large, n % 2U ? 'a' : 'b', 32768U);
            if (UmiUiDocumentViewModelUpsertText(model, &view, large, 32768U) != UMI_STATUS_OK) { publishFailed = 1; break; }
        }
        atomic_store(&read.finished, 1);
        UmiStatus joined = umi_thread_join(thread, &result);
        umi_thread_destroy(thread);
        CHECK(!publishFailed && joined == UMI_STATUS_OK && result == 0);
        return 0;
    }
    memcpy(large + length - 8U, "END-NOTE", 8U);
    CHECK(UmiUiDocumentViewModelUpsertText(model, &view, large, length) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(model, view.view_id, &view) == UMI_STATUS_OK);
    CHECK(UmiUiDocumentViewModelTextInfo(model, view.view_id, &beforeInfo) == UMI_STATUS_OK);
    CHECK(beforeInfo.byte_count == length && !beforeInfo.preview_complete);
    if (strcmp(name, "complete-copy") == 0) return Equals(model, view.view_id, large, length);
    if (strcmp(name, "metadata") == 0) {
        CHECK(umi_ui_document_view_model_set_pinned(model, view.view_id, 1) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_set_word_wrap(model, view.view_id, 1) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_activate(model, view.view_id) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(model, view.view_id, &view) == UMI_STATUS_OK);
        view.cursor_offset = length - 8U; view.selection_length = 8U;
        strcpy(view.uri, "file:///notes-renamed.c");
        CHECK(umi_ui_document_view_model_upsert(model, &view) == UMI_STATUS_OK);
        CHECK(UmiUiDocumentViewModelTextInfo(model, view.view_id, &info) == UMI_STATUS_OK);
        CHECK(info.text_revision == beforeInfo.text_revision);
        return Equals(model, view.view_id, large, length);
    }
    if (strcmp(name, "legacy-edit-rejected") == 0) {
        uint64_t revision = umi_ui_document_view_model_revision(model);
        strcpy(view.source_text, "shorter draft");
        CHECK(umi_ui_document_view_model_upsert(model, &view) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_ui_document_view_model_revision(model) == revision);
        return Equals(model, view.view_id, large, length);
    }
    if (strcmp(name, "explicit-shrink") == 0) {
        CHECK(UmiUiDocumentViewModelUpsertText(model, &view, "", 0U) == UMI_STATUS_OK);
        CHECK(UmiUiDocumentViewModelTextInfo(model, view.view_id, &info) == UMI_STATUS_OK && info.preview_complete && info.byte_count == 0U);
        CHECK(Equals(model, view.view_id, "", 0U) == 0);
        strcpy(view.source_text, "small again");
        CHECK(umi_ui_document_view_model_upsert(model, &view) == UMI_STATUS_OK);
        return Equals(model, view.view_id, "small again", 11U);
    }
    if (strcmp(name, "tail-revision") == 0) {
        large[length - 1U] = '!';
        CHECK(UmiUiDocumentViewModelUpsertText(model, &view, large, length) == UMI_STATUS_OK);
        CHECK(UmiUiDocumentViewModelTextInfo(model, view.view_id, &info) == UMI_STATUS_OK && info.text_revision != beforeInfo.text_revision);
        CHECK(umi_ui_document_view_model_find(model, view.view_id, &after) == UMI_STATUS_OK);
        CHECK(strcmp(view.source_text, after.source_text) == 0);
        return Equals(model, view.view_id, large, length);
    }
    if (strcmp(name, "reorder") == 0) {
        after = View("other"); strcpy(after.source_text, "other");
        CHECK(umi_ui_document_view_model_upsert(model, &after) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_place(model, view.view_id, UMI_UI_SECONDARY_EDITOR_GROUP_ID, 0U) == UMI_STATUS_OK);
        CHECK(Equals(model, view.view_id, large, length) == 0);
        CHECK(Equals(model, after.view_id, "other", 5U) == 0);
        CHECK(umi_ui_document_view_model_place(model, view.view_id, UMI_UI_PRIMARY_EDITOR_GROUP_ID, 1U) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_remove(model, after.view_id) == UMI_STATUS_OK);
        return Equals(model, view.view_id, large, length);
    }
    if (strcmp(name, "preview-reuse") == 0) {
        view.preview = 1; view.pinned = 0; view.dirty = 0;
        CHECK(umi_ui_document_view_model_upsert(model, &view) == UMI_STATUS_OK);
        after = View("preview"); strcpy(after.source_text, "preview");
        CHECK(umi_ui_document_view_model_open_preview(model, &after) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_count(model) == 1U);
        CHECK(UmiUiDocumentViewModelTextInfo(model, "notes", &info) == UMI_STATUS_NOT_FOUND);
        return Equals(model, after.view_id, "preview", 7U);
    }
    if (strcmp(name, "invalid-input") == 0) {
        uint64_t revision = umi_ui_document_view_model_revision(model);
        CHECK(UmiUiDocumentViewModelUpsertText(model, &view, "a\0b", 3U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiUiDocumentViewModelUpsertText(model, &view, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        after = view; memset(after.view_id, 'x', sizeof after.view_id);
        CHECK(UmiUiDocumentViewModelUpsertText(model, &after, large, length) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ui_document_view_model_revision(model) == revision);
        return Equals(model, view.view_id, large, length);
    }
    if (strcmp(name, "reserve") == 0) {
        uint64_t revision = umi_ui_document_view_model_revision(model);
        CHECK(UmiUiDocumentViewModelReserveText(model, view.view_id, length * 2U) == UMI_STATUS_OK);
        CHECK(UmiUiDocumentViewModelReserveText(model, view.view_id, SIZE_MAX) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiUiDocumentViewModelTextInfo(model, view.view_id, &info) == UMI_STATUS_OK && info.text_revision == beforeInfo.text_revision);
        CHECK(umi_ui_document_view_model_revision(model) == revision);
        return Equals(model, view.view_id, large, length);
    }
    if (strcmp(name, "owned-copy") == 0) {
        char *copy = NULL; size_t copied = 0U;
        CHECK(UmiUiDocumentViewModelCopyText(model, view.view_id, &copy, &copied) == UMI_STATUS_OK);
        UmiStatus status = umi_ui_document_view_model_remove(model, view.view_id);
        int intact = status == UMI_STATUS_OK && copied == length && memcmp(copy, large, length) == 0;
        UmiUiDocumentViewModelFreeText(copy);
        CHECK(intact);
        return 0;
    }
    if (strcmp(name, "new-identity") == 0) {
        strcpy(view.document_id, "another-document"); strcpy(view.source_text, "replacement");
        CHECK(umi_ui_document_view_model_upsert(model, &view) == UMI_STATUS_OK);
        return Equals(model, view.view_id, "replacement", 11U);
    }
    fprintf(stderr, "Unknown case: %s\n", name);
    return 1;
}

/* Release model-owned and caller-owned storage even when a case fails. */
int main(int argc, char **argv)
{
    UmiUiDocumentViewModel *model = NULL;
    char *large = malloc(UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U);
    int result = 1;
    if (argc == 2 && large != NULL && umi_ui_document_view_model_create(&model) == UMI_STATUS_OK)
        result = Run(model, large, argv[1]);
    umi_ui_document_view_model_destroy(model);
    free(large);
    return result;
}
