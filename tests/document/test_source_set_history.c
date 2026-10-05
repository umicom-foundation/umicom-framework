/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_source_set_history.c
 * PURPOSE: Verify atomic reviewed source sets keep independent per-document selection history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/source_batch.h"
static int Select(ReplacementFixture *f, size_t cursor, size_t selected)
{
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f->workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, f->viewId, &view) == UMI_STATUS_OK);
    view.cursor_offset = cursor;
    view.selection_length = selected;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    return 0;
}
static int Position(ReplacementFixture *f, size_t cursor, size_t selected)
{
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), f->viewId, &view) ==
              UMI_STATUS_OK &&
          view.cursor_offset == cursor && view.selection_length == selected);
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1],
               *cases[] = {"selections",    "carets",          "unicode", "pending-first", "pending-both",
                           "reverse-order", "unchanged-first", "redo",    "active-tab"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    CHECK(known);
    ReplacementFixture first = {0}, second = {0};
    const char *left = "alpha", *right = "bravo";
    size_t left_cursor = 1U, left_selected = 2U, right_cursor = 2U, right_selected = 1U;
    if (strcmp(name, "carets") == 0)
        left_selected = right_selected = 0U;
    if (strcmp(name, "unicode") == 0)
    {
        left = "a\xe9\x9b\xaa";
        left_selected = 3U;
        right = "b\xf0\x9f\x8c\x8d";
        right_cursor = 1U;
        right_selected = 4U;
    }
    CHECK(Start(&first, left) == 0);
    second = first;
    CHECK(umi_document_coordinator_new(first.documents, "second.c", second.viewId, sizeof(second.viewId)) ==
          UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot state;
    CHECK(umi_document_coordinator_active_snapshot(first.documents, &state) == UMI_STATUS_OK);
    second.id = state.document_id;
    CHECK(Draft(&second, right) == UMI_STATUS_OK &&
          umi_document_coordinator_sync_active(first.documents) == UMI_STATUS_OK);
    int pending_left = strcmp(name, "pending-first") == 0 || strcmp(name, "pending-both") == 0,
        pending_right = strcmp(name, "pending-both") == 0;
    if (pending_left)
    {
        left = "alpha draft";
        CHECK(Draft(&first, left) == UMI_STATUS_OK);
    }
    if (pending_right)
    {
        right = "bravo draft";
        CHECK(Draft(&second, right) == UMI_STATUS_OK);
    }
    CHECK(Select(&first, left_cursor, left_selected) == 0 &&
          Select(&second, right_cursor, right_selected) == 0);
    UmiDocumentId ids[2] = {first.id, second.id};
    size_t left_index = 0U, right_index = 1U;
    if (strcmp(name, "reverse-order") == 0)
    {
        ids[0] = second.id;
        ids[1] = first.id;
        left_index = 1U;
        right_index = 0U;
    }
    UmiDocumentSourceBatch *batch = NULL;
    UmiDocumentSourceBatchSummary summary;
    CHECK(UmiDocumentSourceBatchCreate(first.documents, ids, 2U, &batch) == UMI_STATUS_OK);
    const char *left_after = strcmp(name, "unchanged-first") == 0 ? left : "A", *right_after = "B";
    CHECK(UmiDocumentSourceBatchInspect(batch, &summary) == UMI_STATUS_OK);
    CHECK(UmiDocumentSourceBatchStage(batch, left_index, summary.revision, left_after, strlen(left_after),
                                      strlen(left_after)) == UMI_STATUS_OK);
    CHECK(UmiDocumentSourceBatchInspect(batch, &summary) == UMI_STATUS_OK);
    CHECK(UmiDocumentSourceBatchStage(batch, right_index, summary.revision, right_after, 1U, 1U) ==
          UMI_STATUS_OK);
    CHECK(UmiDocumentSourceBatchInspect(batch, &summary) == UMI_STATUS_OK);
    CHECK(UmiDocumentSourceBatchApply(first.documents, batch, summary.revision, 1) == UMI_STATUS_OK);
    CHECK(ExpectText(&first, left_after) == 0 && ExpectText(&second, right_after) == 0);
    CHECK(umi_document_coordinator_active_snapshot(first.documents, &state) == UMI_STATUS_OK &&
          state.document_id == second.id);
    if (strcmp(name, "unchanged-first") != 0)
    {
        CHECK(UmiDocumentCoordinatorUndo(first.documents, first.id) == UMI_STATUS_OK &&
              ExpectText(&first, left) == 0 && Position(&first, left_cursor, left_selected) == 0);
        CHECK(ExpectText(&second, right_after) == 0 && Position(&second, 1U, 0U) == 0);
    }
    CHECK(UmiDocumentCoordinatorUndo(first.documents, second.id) == UMI_STATUS_OK &&
          ExpectText(&second, right) == 0 && Position(&second, right_cursor, right_selected) == 0);
    if (strcmp(name, "redo") == 0)
    {
        CHECK(UmiDocumentCoordinatorRedo(first.documents, first.id) == UMI_STATUS_OK &&
              Position(&first, 1U, 0U) == 0);
        CHECK(UmiDocumentCoordinatorRedo(first.documents, second.id) == UMI_STATUS_OK &&
              Position(&second, 1U, 0U) == 0);
    }
    if (pending_left)
        CHECK(UmiDocumentCoordinatorUndo(first.documents, first.id) == UMI_STATUS_OK &&
              ExpectText(&first, "alpha") == 0);
    if (pending_right)
        CHECK(UmiDocumentCoordinatorUndo(first.documents, second.id) == UMI_STATUS_OK &&
              ExpectText(&second, "bravo") == 0);
    if (strcmp(name, "active-tab") == 0)
        CHECK(umi_document_coordinator_active_snapshot(first.documents, &state) == UMI_STATUS_OK &&
              state.document_id == second.id);
    UmiDocumentSourceBatchDestroy(batch);
    Stop(&first);
    return 0;
}
