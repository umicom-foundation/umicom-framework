/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_document_identity.c
 *
 * PURPOSE:
 *   Verify identity adoption preserves complete drafts and rejects partial batches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/document_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    const char *cases[] = {"bind", "large", "mixed", "noop", "empty", "stale", "missing",
        "foreign", "duplicate", "unterminated", "invalid-id", "capacity", "arguments"};
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (!strcmp(name, cases[i])) known = 1;
    CHECK(known);
    UmiUiDocumentViewModel *model = NULL;
    CHECK(umi_ui_document_view_model_create(&model) == UMI_STATUS_OK);
    size_t length = !strcmp(name, "large") ? 131072U : 40U;
    char *text = malloc(length + 1U);
    CHECK(text != NULL);
    memset(text, 'x', length);
    text[length - 1U] = 'z'; text[length] = '\0';
    UmiUiDocumentIdentityBinding bindings[2] = {0};
    UmiUiDocumentViewSnapshot original[2];
    UmiUiDocumentTextInfo before[2];
    for (size_t i = 0U; i < 2U; ++i) {
        UmiUiDocumentViewSnapshot view = {0};
        snprintf(view.view_id, sizeof view.view_id, "view.%zu", i);
        snprintf(view.title, sizeof view.title, "Contributed %zu", i);
        strcpy(view.document_id, i == 0U ? "legacy.document" : "");
        strcpy(view.uri, "umicom://sample");
        view.cursor_offset = 3U; view.selection_length = 5U;
        view.dirty = 1; view.read_only = 1; view.pinned = 1;
        CHECK(UmiUiDocumentViewModelUpsertText(model, &view, text, length) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(model, view.view_id, &original[i]) == UMI_STATUS_OK);
        CHECK(UmiUiDocumentViewModelTextInfo(model, view.view_id, &before[i]) == UMI_STATUS_OK);
        strcpy(bindings[i].view_id, view.view_id);
        strcpy(bindings[i].expected_document_id, view.document_id);
        snprintf(bindings[i].document_id, sizeof bindings[i].document_id, "document.%zu", i + 1U);
    }
    uint64_t revision = umi_ui_document_view_model_revision(model), expected = revision;
    size_t count = 2U;
    UmiStatus wanted = UMI_STATUS_OK;
    if (!strcmp(name, "mixed")) {
        strcpy(bindings[0].document_id, bindings[0].expected_document_id);
    } else if (!strcmp(name, "noop")) {
        /* Both destinations are valid; keep the second initially unbound view
         * outside this no-op transaction. */
        strcpy(bindings[0].document_id, bindings[0].expected_document_id); count = 1U;
    } else if (!strcmp(name, "empty")) count = 0U;
    else if (!strcmp(name, "stale")) { expected--; wanted = UMI_STATUS_BUSY; }
    else if (!strcmp(name, "missing")) { strcpy(bindings[1].view_id, "absent"); wanted = UMI_STATUS_NOT_FOUND; }
    else if (!strcmp(name, "foreign")) { strcpy(bindings[1].expected_document_id, "replaced"); wanted = UMI_STATUS_BUSY; }
    else if (!strcmp(name, "duplicate")) { strcpy(bindings[1].view_id, bindings[0].view_id); wanted = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(name, "unterminated")) { memset(bindings[1].expected_document_id, 'x', sizeof bindings[1].expected_document_id); wanted = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(name, "invalid-id")) { bindings[1].document_id[0] = '\0'; wanted = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(name, "capacity")) { count = UMI_UI_DOCUMENT_VIEW_MAX + 1U; wanted = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(name, "arguments")) {
        CHECK(UmiUiDocumentViewModelBindIdentities(NULL, revision, bindings, 2U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiUiDocumentViewModelBindIdentities(model, revision, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        count = 0U;
    }
    CHECK(UmiUiDocumentViewModelBindIdentities(model, expected, count == 0U ? NULL : bindings, count) == wanted);
    int changed = wanted == UMI_STATUS_OK && count == 2U;
    CHECK(umi_ui_document_view_model_revision(model) == revision + (changed ? 1U : 0U));
    for (size_t i = 0U; i < 2U; ++i) {
        UmiUiDocumentViewSnapshot after;
        UmiUiDocumentTextInfo info;
        CHECK(umi_ui_document_view_model_find(model, original[i].view_id, &after) == UMI_STATUS_OK);
        CHECK(UmiUiDocumentViewModelTextInfo(model, original[i].view_id, &info) == UMI_STATUS_OK);
        CHECK(!strcmp(after.document_id, changed ? bindings[i].document_id : original[i].document_id));
        CHECK(info.text_revision == before[i].text_revision +
              (changed && strcmp(bindings[i].document_id, original[i].document_id) ? 1U : 0U));
        CHECK(info.byte_count == length);
        /* Compare fields explicitly: C struct padding is not observable state. */
        CHECK(!strcmp(after.view_id, original[i].view_id) &&
              !strcmp(after.title, original[i].title) &&
              !strcmp(after.icon_name, original[i].icon_name) &&
              !strcmp(after.uri, original[i].uri) &&
              !strcmp(after.language_id, original[i].language_id) &&
              !strcmp(after.group_id, original[i].group_id) &&
              !strcmp(after.source_text, original[i].source_text));
        CHECK(after.dirty == original[i].dirty && after.active == original[i].active &&
              after.pinned == original[i].pinned && after.order == original[i].order &&
              after.cursor_offset == original[i].cursor_offset &&
              after.selection_length == original[i].selection_length &&
              after.preview == original[i].preview && after.read_only == original[i].read_only &&
              after.closable == original[i].closable && after.word_wrap == original[i].word_wrap &&
              after.show_line_numbers == original[i].show_line_numbers);
        char *copy = NULL; size_t size = 0U;
        CHECK(UmiUiDocumentViewModelCopyText(model, after.view_id, &copy, &size) == UMI_STATUS_OK);
        CHECK(size == length && !memcmp(copy, text, length + 1U));
        UmiUiDocumentViewModelFreeText(copy);
    }
    free(text);
    umi_ui_document_view_model_destroy(model);
    return 0;
}
