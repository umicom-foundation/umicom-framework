/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_source_inspection.c
 * PURPOSE: Verify read-only source capture without granting editing permission or changing document history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/source_request.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *cases[] = {
        "read-only",       "writable",          "empty",         "owned",        "stage-denied",
        "apply-denied",    "permission-change", "source-change", "caret-change", "selection-change",
        "language-change", "other-owner",       "other-tab",     "arguments",    "edit-refused"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    ReplacementFixture fixture = {0}, other = {0};
    const char *original = strcmp(mode, "empty") == 0 ? "" : "source\n";
    CHECK(Start(&fixture, original) == 0);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(fixture.workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK);
    view.read_only = strcmp(mode, "writable") == 0 ? 0 : 1;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot before, after;
    CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &before) == UMI_STATUS_OK);
    UmiDocumentSourceRequest *request = NULL;
    CHECK(UmiDocumentSourceRequestCreateInspection(fixture.documents, fixture.id, &request) == UMI_STATUS_OK);
    UmiDocumentSourceRequestSummary summary;
    CHECK(UmiDocumentSourceRequestInspect(request, &summary) == UMI_STATUS_OK);
    CHECK(!summary.has_proposal && !summary.applied && summary.source_bytes == strlen(original) &&
          summary.revision == 1U);
    CHECK(UmiDocumentSourceRequestCheck(fixture.documents, request) == UMI_STATUS_OK);
    const char *text = NULL;
    size_t bytes = 0U;
    CHECK(UmiDocumentSourceRequestRead(request, &text, &bytes) == UMI_STATUS_OK);
    CHECK(bytes == strlen(original) && strcmp(text, original) == 0);
    /* Even a capture made from a writable document must never be promoted to
     * an editable request after the user chose a read-only operation. */
    CHECK(UmiDocumentSourceRequestStage(request, summary.revision, "replacement", 11U, 0U) ==
          UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiDocumentSourceRequestApply(fixture.documents, request, summary.revision, 1) ==
          UMI_STATUS_PERMISSION_DENIED);
    const char *proposal = text;
    size_t proposed = 99U;
    CHECK(UmiDocumentSourceRequestProposed(request, &proposal, &proposed) == UMI_STATUS_INVALID_STATE &&
          proposal == NULL && proposed == 0U);
    if (strcmp(mode, "stage-denied") == 0)
        CHECK(UmiDocumentSourceRequestStage(request, UINT64_MAX, text, bytes, bytes) ==
              UMI_STATUS_PERMISSION_DENIED);
    if (strcmp(mode, "apply-denied") == 0)
    {
        CHECK(UmiDocumentSourceRequestApply(fixture.documents, request, UINT64_MAX, 1) ==
              UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiDocumentSourceRequestApply(fixture.documents, request, 1U, 0) ==
              UMI_STATUS_PERMISSION_DENIED);
    }
    if (strcmp(mode, "owned") == 0 || strcmp(mode, "source-change") == 0)
    {
        CHECK(Draft(&fixture, "new draft") == UMI_STATUS_OK);
        CHECK(strcmp(text, original) == 0);
        CHECK(UmiDocumentSourceRequestCheck(fixture.documents, request) == UMI_STATUS_INVALID_STATE);
    }
    else if (strcmp(mode, "permission-change") == 0 || strcmp(mode, "caret-change") == 0 ||
             strcmp(mode, "selection-change") == 0 || strcmp(mode, "language-change") == 0)
    {
        if (strcmp(mode, "permission-change") == 0)
            view.read_only = 0;
        if (strcmp(mode, "caret-change") == 0)
            view.cursor_offset = 1U;
        if (strcmp(mode, "selection-change") == 0)
            view.selection_length = 1U;
        if (strcmp(mode, "language-change") == 0)
            strcpy(view.language_id, "different");
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
        CHECK(UmiDocumentSourceRequestCheck(fixture.documents, request) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentSourceRequestStage(request, 1U, "replacement", 11U, 0U) ==
              UMI_STATUS_PERMISSION_DENIED);
    }
    else if (strcmp(mode, "other-owner") == 0)
    {
        CHECK(Start(&other, original) == 0);
        CHECK(UmiDocumentSourceRequestCheck(other.documents, request) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "other-tab") == 0)
    {
        CHECK(umi_document_coordinator_new(fixture.documents, "other.c", NULL, 0U) == UMI_STATUS_OK);
        CHECK(UmiDocumentSourceRequestCheck(fixture.documents, request) == UMI_STATUS_OK);
    }
    else if (strcmp(mode, "edit-refused") == 0)
    {
        UmiDocumentSourceRequest *edit = NULL;
        CHECK(UmiDocumentSourceRequestCreate(fixture.documents, fixture.id, &edit) ==
                  UMI_STATUS_PERMISSION_DENIED &&
              edit == NULL);
    }
    else if (strcmp(mode, "arguments") == 0)
    {
        UmiDocumentSourceRequest *bad = request;
        CHECK(UmiDocumentSourceRequestCreateInspection(NULL, fixture.id, &bad) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              bad == NULL);
        CHECK(UmiDocumentSourceRequestCreateInspection(fixture.documents, 0U, &bad) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              bad == NULL);
        CHECK(UmiDocumentSourceRequestCreateInspection(fixture.documents, fixture.id, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    CHECK(UmiDocumentSourceRequestInspect(request, &summary) == UMI_STATUS_OK && !summary.has_proposal &&
          summary.revision == 1U);
    if (strcmp(mode, "other-tab") != 0)
    {
        CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &after) == UMI_STATUS_OK);
        CHECK(after.undo_count == before.undo_count && after.redo_count == before.redo_count);
    }
    CHECK(ExpectText(&fixture, strcmp(mode, "owned") == 0 || strcmp(mode, "source-change") == 0
                                   ? "new draft"
                                   : original) == 0);
    UmiDocumentSourceRequestDestroy(request);
    if (other.documents != NULL)
        Stop(&other);
    Stop(&fixture);
    return 0;
}
