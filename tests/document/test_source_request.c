/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_source_request.c
 * PURPOSE: Verify captured asynchronous source edits, stale-state refusal and document Undo.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/source_request.h"

static UmiStatus Cursor(ReplacementFixture *fixture, size_t offset, size_t selection)
{
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(fixture->workbench);
    UmiUiDocumentViewSnapshot view;
    UmiStatus status = umi_ui_document_view_model_find(views, fixture->viewId, &view);
    if (status == UMI_STATUS_OK)
    {
        view.cursor_offset = offset;
        view.selection_length = selection;
        status = umi_ui_document_view_model_upsert(views, &view);
    }
    return status;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    const char *cases[] = {"capture",     "empty",        "owned",     "apply",          "delete",
                           "unchanged",   "undo",         "redo",      "pending-typing", "stale",
                           "round-trip",  "caret",        "selection", "read-only",      "closed",
                           "other-owner", "other-tab",    "approval",  "revision",       "restage",
                           "unicode",     "invalid-utf8", "nul",       "cursor-utf8",    "cursor-crlf",
                           "limits",      "after-apply",  "arguments"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    CHECK(known);
    ReplacementFixture fixture = {0};
    const char *original = strcmp(name, "empty") == 0 ? "" : "source\n";
    CHECK(Start(&fixture, original) == 0);
    if (strcmp(name, "pending-typing") == 0)
    {
        original = "source pending\n";
        CHECK(Draft(&fixture, original) == UMI_STATUS_OK);
    }
    CHECK(Cursor(&fixture, original[0] == '\0' ? 0U : 2U, 0U) == UMI_STATUS_OK);
    UmiDocumentSourceRequest *request = NULL;
    CHECK(UmiDocumentSourceRequestCreate(fixture.documents, fixture.id, &request) == UMI_STATUS_OK);
    UmiDocumentSourceRequestSummary summary;
    CHECK(UmiDocumentSourceRequestInspect(request, &summary) == UMI_STATUS_OK);
    CHECK(summary.document_id == fixture.id && summary.source_bytes == strlen(original) &&
          summary.cursor_offset == (original[0] == '\0' ? 0U : 2U) && !summary.has_proposal);
    const char *source = NULL;
    size_t bytes = 0U;
    CHECK(UmiDocumentSourceRequestRead(request, &source, &bytes) == UMI_STATUS_OK);
    CHECK(bytes == strlen(original) && memcmp(source, original, bytes) == 0);
    CHECK(UmiDocumentSourceRequestCheck(fixture.documents, request) == UMI_STATUS_OK);
    const char *replacement = strcmp(name, "delete") == 0      ? ""
                              : strcmp(name, "unchanged") == 0 ? original
                              : strcmp(name, "unicode") == 0   ? "x\xf0\x9f\x98\x80"
                                                               : "complete\n";
    size_t cursor = strlen(replacement);
    if (strcmp(name, "unchanged") == 0)
        cursor = 3U;
    CHECK(UmiDocumentSourceRequestStage(request, summary.revision, replacement, strlen(replacement),
                                        cursor) == UMI_STATUS_OK);
    CHECK(UmiDocumentSourceRequestInspect(request, &summary) == UMI_STATUS_OK);
    CHECK(summary.has_proposal && summary.revision == 2U);
    uint64_t reviewed = summary.revision;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(name, "capture") == 0 || strcmp(name, "owned") == 0)
    {
        CHECK(ExpectText(&fixture, original) == 0);
        if (strcmp(name, "owned") == 0)
        {
            CHECK(Draft(&fixture, "later") == UMI_STATUS_OK);
            CHECK(strcmp(source, original) == 0);
        }
        goto cleanup;
    }
    if (strcmp(name, "stale") == 0 || strcmp(name, "round-trip") == 0)
    {
        CHECK(Draft(&fixture, "later") == UMI_STATUS_OK);
        if (strcmp(name, "round-trip") == 0)
        {
            CHECK(Draft(&fixture, original) == UMI_STATUS_OK);
            CHECK(Cursor(&fixture, 2U, 0U) == UMI_STATUS_OK);
        }
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (strcmp(name, "caret") == 0 || strcmp(name, "selection") == 0)
    {
        CHECK(Cursor(&fixture, strcmp(name, "caret") == 0 ? 3U : 2U,
                     strcmp(name, "selection") == 0 ? 1U : 0U) == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (strcmp(name, "read-only") == 0)
    {
        UmiUiDocumentViewSnapshot view;
        CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(fixture.workbench), fixture.viewId,
                                              &view) == UMI_STATUS_OK);
        view.read_only = 1;
        CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(fixture.workbench), &view) ==
              UMI_STATUS_OK);
        expected = UMI_STATUS_PERMISSION_DENIED;
    }
    else if (strcmp(name, "closed") == 0)
    {
        CHECK(umi_document_coordinator_close_active(fixture.documents, 1) == UMI_STATUS_OK);
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(name, "other-owner") == 0)
    {
        ReplacementFixture other = {0};
        CHECK(Start(&other, "different") == 0);
        CHECK(UmiDocumentSourceRequestApply(other.documents, request, reviewed, 1) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ExpectText(&other, "different") == 0);
        Stop(&other);
        goto cleanup;
    }
    else if (strcmp(name, "approval") == 0)
    {
        CHECK(UmiDocumentSourceRequestApply(fixture.documents, request, reviewed, 0) ==
              UMI_STATUS_PERMISSION_DENIED);
        CHECK(ExpectText(&fixture, original) == 0);
        goto cleanup;
    }
    else if (strcmp(name, "revision") == 0)
    {
        --reviewed;
        expected = UMI_STATUS_BUSY;
    }
    else if (strcmp(name, "restage") == 0)
    {
        CHECK(UmiDocumentSourceRequestStage(request, reviewed, "second\n", 7U, 7U) == UMI_STATUS_OK);
        CHECK(UmiDocumentSourceRequestApply(fixture.documents, request, reviewed, 1) == UMI_STATUS_BUSY);
        CHECK(UmiDocumentSourceRequestInspect(request, &summary) == UMI_STATUS_OK);
        reviewed = summary.revision;
        replacement = "second\n";
    }
    else if (strcmp(name, "invalid-utf8") == 0 || strcmp(name, "nul") == 0 ||
             strcmp(name, "cursor-utf8") == 0 || strcmp(name, "cursor-crlf") == 0 ||
             strcmp(name, "limits") == 0)
    {
        const char *bad = strcmp(name, "invalid-utf8") == 0  ? "\xff"
                          : strcmp(name, "nul") == 0         ? "x\0y"
                          : strcmp(name, "cursor-utf8") == 0 ? "\xf0\x9f\x98\x80"
                                                             : "a\r\nb";
        size_t bad_bytes = strcmp(name, "nul") == 0 ? 3U : strlen(bad);
        size_t bad_cursor = strcmp(name, "cursor-utf8") == 0 ? 1U : 2U;
        UmiStatus rejected = UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(name, "limits") == 0)
        {
            bad_bytes = (size_t)UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U;
            rejected = UMI_STATUS_CAPACITY_EXCEEDED;
        }
        CHECK(UmiDocumentSourceRequestStage(request, reviewed, bad, bad_bytes, bad_cursor) == rejected);
        const char *proposed = NULL;
        size_t proposed_bytes;
        CHECK(UmiDocumentSourceRequestProposed(request, &proposed, &proposed_bytes) == UMI_STATUS_OK);
        CHECK(proposed_bytes == strlen(replacement) && strcmp(proposed, replacement) == 0);
        CHECK(UmiDocumentSourceRequestInspect(request, &summary) == UMI_STATUS_OK &&
              summary.revision == reviewed);
    }
    else if (strcmp(name, "arguments") == 0)
    {
        UmiDocumentSourceRequest *invalid = NULL;
        CHECK(UmiDocumentSourceRequestCreate(NULL, fixture.id, &invalid) == UMI_STATUS_INVALID_ARGUMENT &&
              invalid == NULL);
        CHECK(UmiDocumentSourceRequestRead(NULL, &source, &bytes) == UMI_STATUS_INVALID_ARGUMENT &&
              source == NULL && bytes == 0U);
        CHECK(UmiDocumentSourceRequestStage(request, reviewed, NULL, 0U, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }
    char other_view[UMI_UI_ID_CAPACITY] = {0};
    if (strcmp(name, "other-tab") == 0)
        CHECK(umi_document_coordinator_new(fixture.documents, "other.c", other_view, sizeof(other_view)) ==
              UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot before_snapshot;
    if (strcmp(name, "unchanged") == 0)
        CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &before_snapshot) == UMI_STATUS_OK);
    CHECK(UmiDocumentSourceRequestApply(fixture.documents, request, reviewed, 1) == expected);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(ExpectText(&fixture, replacement) == 0);
        if (strcmp(name, "other-tab") == 0)
        {
            UmiDocumentWorkingCopySnapshot active;
            CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &active) == UMI_STATUS_OK);
            CHECK(strcmp(active.view_id, other_view) == 0 && active.document_id != fixture.id);
        }
        if (strcmp(name, "unchanged") == 0)
        {
            UmiDocumentWorkingCopySnapshot after_snapshot;
            CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &after_snapshot) ==
                  UMI_STATUS_OK);
            CHECK(before_snapshot.undo_count == after_snapshot.undo_count);
        }
        if (strcmp(name, "undo") == 0 || strcmp(name, "redo") == 0 || strcmp(name, "pending-typing") == 0)
        {
            CHECK(UmiDocumentCoordinatorUndo(fixture.documents, fixture.id) == UMI_STATUS_OK);
            CHECK(ExpectText(&fixture, original) == 0);
            if (strcmp(name, "pending-typing") == 0)
            {
                CHECK(UmiDocumentCoordinatorUndo(fixture.documents, fixture.id) == UMI_STATUS_OK);
                CHECK(ExpectText(&fixture, "source\n") == 0);
            }
            if (strcmp(name, "redo") == 0)
            {
                CHECK(UmiDocumentCoordinatorRedo(fixture.documents, fixture.id) == UMI_STATUS_OK);
                CHECK(ExpectText(&fixture, replacement) == 0);
            }
        }
        if (strcmp(name, "after-apply") == 0)
        {
            CHECK(UmiDocumentSourceRequestApply(fixture.documents, request, reviewed, 1) ==
                  UMI_STATUS_INVALID_STATE);
            CHECK(UmiDocumentSourceRequestRead(request, &source, &bytes) == UMI_STATUS_OK &&
                  strcmp(source, original) == 0);
        }
    }
    else if (strcmp(name, "stale") != 0 && strcmp(name, "closed") != 0)
        CHECK(ExpectText(&fixture, original) == 0);
cleanup:
    UmiDocumentSourceRequestDestroy(request);
    Stop(&fixture);
    return 0;
}
