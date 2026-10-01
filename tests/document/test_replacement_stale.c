/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_replacement_stale.c
 * PURPOSE: Reject changed review targets without overwriting later text or history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"

/* Mutate one independent owner dimension after preparing the review. */
static int Run(ReplacementFixture *f, const char *name)
{
    CHECK(UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, "note", "saved", &f->plan) == UMI_STATUS_OK);
    UmiStatus expected = UMI_STATUS_INVALID_STATE;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f->workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, f->viewId, &view) == UMI_STATUS_OK);
    if (strcmp(name, "draft") == 0) CHECK(Draft(f, "later typing") == UMI_STATUS_OK);
    else if (strcmp(name, "round-trip") == 0) {
        CHECK(Draft(f, "temporary") == UMI_STATUS_OK);
        CHECK(Draft(f, "note note") == UMI_STATUS_OK);
    } else if (strcmp(name, "store") == 0)
        CHECK(umi_document_store_replace_text(f->store, f->id, "new store", 9U) == UMI_STATUS_OK);
    else if (strcmp(name, "saved-path") == 0)
        CHECK(umi_document_store_mark_saved_as(f->store, f->id, "replacement-review-target.txt") == UMI_STATUS_OK);
    else if (strcmp(name, "marker") == 0)
        CHECK(umi_document_store_mark_external_change(f->store, f->id, 1) == UMI_STATUS_OK);
    else if (strcmp(name, "read-only") == 0) {
        view.read_only = 1; expected = UMI_STATUS_PERMISSION_DENIED;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    } else if (strcmp(name, "closed") == 0) {
        CHECK(UmiDocumentCoordinatorClose(f->documents, f->id, 1) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_new(f->documents, "review.c", NULL, 0U) == UMI_STATUS_OK);
        expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(name, "other-owner") == 0) {
        ReplacementFixture other = {0};
        int ready = Start(&other, "note note");
        UmiStatus wrong = ready == 0 ? UmiDocumentCoordinatorApplyReplacement(other.documents, f->plan, NULL) : UMI_STATUS_OK;
        Stop(&other); CHECK(ready == 0 && wrong == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorCheckReplacement(f->documents, f->plan) == UMI_STATUS_OK);
        return 0;
    } else if (strcmp(name, "uri") == 0) {
        strcpy(view.uri, "untitled:///renamed");
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    } else if (strcmp(name, "view-identity") == 0) {
        strcpy(view.document_id, "different-document");
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    } else if (strcmp(name, "selection-allowed") == 0) {
        view.cursor_offset = 9U; view.selection_length = 0U;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorApplyReplacement(f->documents, f->plan, NULL) == UMI_STATUS_OK);
        CHECK(ExpectText(f, "saved saved") == 0);
        return 0;
    } else return 2;
    UmiDocumentWorkingCopySnapshot original, current;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &original) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorCheckReplacement(f->documents, f->plan) == expected);
    size_t count = 123U;
    CHECK(UmiDocumentCoordinatorApplyReplacement(f->documents, f->plan, &count) == expected && count == 123U);
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &current) == UMI_STATUS_OK);
    CHECK(current.revision == original.revision && current.undo_count == original.undo_count && current.redo_count == original.redo_count);
    CHECK(ExpectPlan(f, "note note", "saved saved", 2U) == 0);
    if (strcmp(name, "closed") != 0)
        CHECK(ExpectText(f, strcmp(name, "draft") == 0 ? "later typing" : "note note") == 0);
    return 0;
}

/* Rejected plans retain their capture until normal caller destruction. */
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    ReplacementFixture f = {0}; int result = Start(&f, "note note");
    if (result == 0) result = Run(&f, argv[1]);
    Stop(&f); return result;
}
