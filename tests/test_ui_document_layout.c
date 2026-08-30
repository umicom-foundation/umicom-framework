/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_document_layout.c
 *
 * PURPOSE:
 *   Verify ordered editor-group capture, persistence and non-destructive
 *   restoration through the public toolkit-neutral Framework contract.
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

static void add_document(UmiUiDocumentViewModel *documents,
                         const char *view_id,
                         const char *group_id,
                         int active)
{
    UmiUiDocumentViewSnapshot document = {0};
    (void)snprintf(document.view_id, sizeof(document.view_id), "%s", view_id);
    (void)snprintf(document.document_id, sizeof(document.document_id), "%s",
                   view_id);
    (void)snprintf(document.title, sizeof(document.title), "%s", view_id);
    (void)snprintf(document.group_id, sizeof(document.group_id), "%s",
                   group_id);
    document.active = active;
    document.closable = 1;
    assert(umi_ui_document_view_model_upsert(documents, &document) ==
           UMI_STATUS_OK);
}

int main(void)
{
    UmiUiDocumentViewModel *documents = NULL;
    UmiUiDocumentLayoutSnapshot saved;
    UmiUiDocumentLayoutApplyResult result;
    UmiUiDocumentLayoutEntry decoded;
    UmiUiDocumentViewSnapshot document;
    char encoded[UMI_UI_DOCUMENT_LAYOUT_ENTRY_TEXT_CAPACITY];

    assert(umi_ui_document_view_model_create(&documents) == UMI_STATUS_OK);
    add_document(documents, "view.alpha", UMI_UI_PRIMARY_EDITOR_GROUP_ID, 1);
    add_document(documents, "view.bravo", UMI_UI_PRIMARY_EDITOR_GROUP_ID, 0);
    add_document(documents, "view.charlie", UMI_UI_SECONDARY_EDITOR_GROUP_ID,
                 1);

    assert(umi_ui_document_view_model_place(
               documents, "view.bravo", UMI_UI_PRIMARY_EDITOR_GROUP_ID, 0U) ==
           UMI_STATUS_OK);
    assert(umi_ui_document_layout_capture(documents, &saved) == UMI_STATUS_OK);
    assert(saved.count == 3U);
    assert(strcmp(saved.entries[0].view_id, "view.bravo") == 0);
    assert(saved.entries[0].position == 0);
    assert(umi_ui_document_layout_entry_encode(
               &saved.entries[0], encoded, sizeof(encoded)) == UMI_STATUS_OK);
    assert(umi_ui_document_layout_entry_decode(encoded, &decoded) ==
           UMI_STATUS_OK);
    assert(strcmp(decoded.group_id, UMI_UI_PRIMARY_EDITOR_GROUP_ID) == 0);

    assert(umi_ui_document_view_model_place(
               documents, "view.bravo", UMI_UI_SECONDARY_EDITOR_GROUP_ID,
               1U) == UMI_STATUS_OK);
    assert(umi_ui_document_layout_apply(documents, &saved, &result) ==
           UMI_STATUS_OK);
    assert(result.applied_count == 3U && result.missing_count == 0U);
    assert(umi_ui_document_view_model_at(documents, 0U, &document) ==
           UMI_STATUS_OK);
    assert(strcmp(document.view_id, "view.bravo") == 0);
    assert(strcmp(document.group_id, UMI_UI_PRIMARY_EDITOR_GROUP_ID) == 0);
    assert(umi_ui_document_view_model_find(documents, "view.charlie",
                                           &document) == UMI_STATUS_OK);
    assert(document.active != 0);

    assert(umi_ui_document_view_model_remove(documents, "view.alpha") ==
           UMI_STATUS_OK);
    assert(umi_ui_document_layout_apply(documents, &saved, &result) ==
           UMI_STATUS_OK);
    assert(result.applied_count == 2U && result.missing_count == 1U);
    assert(umi_ui_document_layout_entry_decode("v1|bad id|editor.primary|0|1",
                                               &decoded) ==
           UMI_STATUS_PARSE_ERROR);

    umi_ui_document_view_model_destroy(documents);
    return EXIT_SUCCESS;
}
