/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_replacement_apply.c
 * PURPOSE: Exercise captured targets, existing undo/redo history and one-use replacement plans.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"

/* A bulk edit is one action on the reviewed draft, including pending typing. */
static int Run(ReplacementFixture *f, const char *name)
{
    const char *before = "note note", *after = "saved saved";
    int pending = strcmp(name, "pending-typing") == 0;
    int noChange = strcmp(name, "no-change") == 0;
    if (pending) { before = "note note draft"; after = "saved saved draft"; CHECK(Draft(f, before) == UMI_STATUS_OK); }
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f->workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, f->viewId, &view) == UMI_STATUS_OK);
    view.cursor_offset = 1U; view.selection_length = 2U;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot original, current;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &original) == UMI_STATUS_OK);
    const char *replacement = noChange ? "note" : "saved";
    if (strcmp(name, "caret") == 0) replacement = "\xc2\xa3";
    CHECK(UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, "note", replacement, &f->plan) == UMI_STATUS_OK);
    if (strcmp(name, "second-plan") == 0)
        CHECK(UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, "note", "different", &f->second) == UMI_STATUS_OK);
    char otherView[UMI_UI_ID_CAPACITY] = {0};
    if (strcmp(name, "inactive") == 0)
        CHECK(umi_document_coordinator_new(f->documents, "other", otherView, sizeof otherView) == UMI_STATUS_OK);
    size_t count = 99U;
    CHECK(UmiDocumentCoordinatorCheckReplacement(f->documents, f->plan) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorApplyReplacement(f->documents, f->plan, &count) == UMI_STATUS_OK && count == 2U);
    CHECK(umi_ui_document_view_model_find(views, f->viewId, &view) == UMI_STATUS_OK);
    if (noChange) {
        CHECK(ExpectText(f, before) == 0 && view.cursor_offset == 1U && view.selection_length == 2U);
        CHECK(umi_document_coordinator_active_snapshot(f->documents, &current) == UMI_STATUS_OK);
        CHECK(current.undo_count == original.undo_count && current.revision == original.revision);
        return 0;
    }
    if (strcmp(name, "caret") == 0) {
        CHECK(ExpectText(f, "\xc2\xa3 \xc2\xa3") == 0);
        CHECK(view.cursor_offset == 0U && view.selection_length == 0U);
        return 0;
    }
    CHECK(ExpectText(f, after) == 0 && view.selection_length == 0U);
    if (strcmp(name, "inactive") == 0) {
        CHECK(umi_document_coordinator_active_snapshot(f->documents, &current) == UMI_STATUS_OK);
        CHECK(strcmp(current.view_id, otherView) == 0 && current.document_id != f->id);
        char *other = NULL; size_t bytes = 99U;
        CHECK(UmiUiDocumentViewModelCopyText(views, otherView, &other, &bytes) == UMI_STATUS_OK);
        UmiUiDocumentViewModelFreeText(other); CHECK(bytes == 0U);
    } else if (strcmp(name, "consumed") == 0) {
        UmiDocumentReplacementSummary summary = {.match_count = 44U};
        CHECK(UmiDocumentReplacementPlanSummary(f->plan, &summary) == UMI_STATUS_INVALID_STATE && summary.match_count == 44U);
        const char *a = "x", *b = "y"; size_t x = 1U, y = 2U;
        CHECK(UmiDocumentReplacementPlanTexts(f->plan, &a, &x, &b, &y) == UMI_STATUS_INVALID_STATE);
        CHECK(a == NULL && b == NULL && x == 0U && y == 0U);
        count = 55U;
        CHECK(UmiDocumentCoordinatorApplyReplacement(f->documents, f->plan, &count) == UMI_STATUS_INVALID_STATE && count == 55U);
    } else if (strcmp(name, "second-plan") == 0) {
        CHECK(UmiDocumentCoordinatorApplyReplacement(f->documents, f->second, NULL) == UMI_STATUS_INVALID_STATE);
        CHECK(ExpectText(f, after) == 0);
    } else if (!pending && strcmp(name, "undo-redo") != 0) return 2;
    CHECK(UmiDocumentCoordinatorUndo(f->documents, f->id) == UMI_STATUS_OK);
    CHECK(ExpectText(f, before) == 0);
    if (pending) {
        CHECK(UmiDocumentCoordinatorUndo(f->documents, f->id) == UMI_STATUS_OK);
        CHECK(ExpectText(f, "note note") == 0);
        CHECK(UmiDocumentCoordinatorRedo(f->documents, f->id) == UMI_STATUS_OK);
        CHECK(ExpectText(f, before) == 0);
    }
    CHECK(UmiDocumentCoordinatorRedo(f->documents, f->id) == UMI_STATUS_OK);
    CHECK(ExpectText(f, after) == 0);
    return 0;
}

/* Fixture teardown also destroys consumed captures. */
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    ReplacementFixture f = {0}; int result = Start(&f, "note note");
    if (result == 0) result = Run(&f, argv[1]);
    Stop(&f); return result;
}
