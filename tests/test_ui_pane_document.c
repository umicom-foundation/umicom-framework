/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_pane_document.c
 *
 * PURPOSE:
 *   Verify pane placement and document-tab state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiPaneModel *panes = NULL;
    UmiUiDocumentViewModel *documents = NULL;
    UmiUiPaneSnapshot pane = {0};
    UmiUiDocumentViewSnapshot document = {0};
    UmiUiDocumentViewSnapshot preview = {0};
    UmiUiDocumentViewSnapshot result = {0};
    UmiUiDocumentCloseResult close_result = {0};
    char next_id[UMI_UI_ID_CAPACITY];
    assert(umi_ui_pane_model_create(&panes) == UMI_STATUS_OK);
    assert(umi_ui_document_view_model_create(&documents) == UMI_STATUS_OK);
    (void)snprintf(pane.pane_id, sizeof(pane.pane_id), "%s", "studio.explorer");
    (void)snprintf(pane.title, sizeof(pane.title), "%s", "Explorer");
    pane.placement = UMI_UI_PLACEMENT_LEFT; pane.visible = 1;
    assert(umi_ui_pane_model_upsert(panes, &pane) == UMI_STATUS_OK);
    (void)snprintf(document.view_id, sizeof(document.view_id), "%s", "view.readme");
    (void)snprintf(document.document_id, sizeof(document.document_id), "%s", "doc.readme");
    (void)snprintf(document.title, sizeof(document.title), "%s", "README.md");
    (void)snprintf(document.group_id, sizeof(document.group_id), "%s", "editor.primary");
    document.active = 1;
    document.pinned = 1;
    document.closable = 1;
    document.show_line_numbers = 1;
    assert(umi_ui_document_view_model_upsert(documents, &document) == UMI_STATUS_OK);
    assert(umi_ui_pane_model_count(panes) == 1U);
    assert(umi_ui_document_view_model_count(documents) == 1U);

    (void)snprintf(preview.view_id, sizeof(preview.view_id), "%s", "view.preview-a");
    (void)snprintf(preview.document_id, sizeof(preview.document_id), "%s", "doc.preview-a");
    (void)snprintf(preview.title, sizeof(preview.title), "%s", "preview-a.c");
    (void)snprintf(preview.group_id, sizeof(preview.group_id), "%s", "editor.primary");
    preview.closable = 1;
    assert(umi_ui_document_view_model_open_preview(documents, &preview) == UMI_STATUS_OK);
    assert(umi_ui_document_view_model_count(documents) == 2U);

    (void)snprintf(preview.view_id, sizeof(preview.view_id), "%s", "view.preview-b");
    (void)snprintf(preview.document_id, sizeof(preview.document_id), "%s", "doc.preview-b");
    (void)snprintf(preview.title, sizeof(preview.title), "%s", "preview-b.c");
    assert(umi_ui_document_view_model_open_preview(documents, &preview) == UMI_STATUS_OK);
    assert(umi_ui_document_view_model_count(documents) == 2U);
    assert(umi_ui_document_view_model_find(documents, "view.preview-a", &result) ==
           UMI_STATUS_NOT_FOUND);
    assert(umi_ui_document_view_model_set_pinned(documents, "view.preview-b", 1) ==
           UMI_STATUS_OK);
    assert(umi_ui_document_view_model_find(documents, "view.preview-b", &result) ==
           UMI_STATUS_OK);
    assert(result.pinned && !result.preview);
    assert(umi_ui_document_view_model_open_preview(documents, &preview) ==
           UMI_STATUS_OK);
    assert(umi_ui_document_view_model_find(documents, "view.preview-b", &result) ==
           UMI_STATUS_OK);
    assert(result.pinned && !result.preview);

    (void)snprintf(preview.view_id, sizeof(preview.view_id), "%s", "view.scratch");
    (void)snprintf(preview.document_id, sizeof(preview.document_id), "%s", "doc.scratch");
    (void)snprintf(preview.title, sizeof(preview.title), "%s", "scratch.c");
    preview.preview = 0;
    preview.pinned = 0;
    preview.active = 0;
    preview.dirty = 0;
    assert(umi_ui_document_view_model_upsert(documents, &preview) == UMI_STATUS_OK);
    (void)snprintf(preview.view_id, sizeof(preview.view_id), "%s", "view.unsaved");
    (void)snprintf(preview.document_id, sizeof(preview.document_id), "%s", "doc.unsaved");
    (void)snprintf(preview.title, sizeof(preview.title), "%s", "unsaved.c");
    preview.dirty = 1;
    assert(umi_ui_document_view_model_upsert(documents, &preview) == UMI_STATUS_OK);

    assert(umi_ui_document_view_model_activate_relative(
               documents, "view.readme", 1, next_id, sizeof(next_id)) == UMI_STATUS_OK);
    assert(strcmp(next_id, "view.preview-b") == 0);
    assert(umi_ui_document_view_model_set_word_wrap(documents, "view.readme", 1) ==
           UMI_STATUS_OK);
    assert(umi_ui_document_view_model_move_to_group(
               documents, "view.unsaved", "editor.secondary") == UMI_STATUS_OK);
    assert(umi_ui_document_view_model_group_count(documents, "editor.primary") == 3U);
    assert(umi_ui_document_view_model_group_count(documents, "editor.secondary") == 1U);

    assert(umi_ui_document_view_model_close_others(
               documents, "view.readme", &close_result) == UMI_STATUS_OK);
    assert(close_result.closed_count == 1U);
    assert(close_result.pinned_count == 1U);
    assert(umi_ui_document_view_model_find(documents, "view.scratch", &result) ==
           UMI_STATUS_NOT_FOUND);
    assert(umi_ui_document_view_model_close_all(documents, &close_result) == UMI_STATUS_OK);
    assert(close_result.dirty_count == 1U);
    assert(close_result.pinned_count == 2U);
    assert(umi_ui_document_view_model_activate_group(
               documents, UMI_UI_SECONDARY_EDITOR_GROUP_ID,
               next_id, sizeof(next_id)) == UMI_STATUS_OK);
    assert(strcmp(next_id, "view.unsaved") == 0);
    assert(umi_ui_document_view_model_merge_group(
               documents,
               UMI_UI_SECONDARY_EDITOR_GROUP_ID,
               UMI_UI_PRIMARY_EDITOR_GROUP_ID) == UMI_STATUS_OK);
    assert(umi_ui_document_view_model_group_count(
               documents, UMI_UI_SECONDARY_EDITOR_GROUP_ID) == 0U);
    assert(umi_ui_document_view_model_group_count(
               documents, UMI_UI_PRIMARY_EDITOR_GROUP_ID) == 3U);
    umi_ui_document_view_model_destroy(documents);
    umi_ui_pane_model_destroy(panes);
    return EXIT_SUCCESS;
}
