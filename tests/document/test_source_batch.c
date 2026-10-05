/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_source_batch.c
 * PURPOSE: Exercise complete draft-set review with real document ownership and per-document Undo.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/source_batch.h"

static int Second(ReplacementFixture *first, ReplacementFixture *second)
{
    *second = *first;
    second->plan = NULL;
    second->second = NULL;
    CHECK(umi_document_coordinator_new(first->documents, "second.c", second->viewId,
                                       sizeof(second->viewId)) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_active_snapshot(first->documents, &snapshot) == UMI_STATUS_OK);
    second->id = snapshot.document_id;
    CHECK(Draft(second, "second") == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_sync_active(first->documents) == UMI_STATUS_OK);
    return 0;
}
static UmiStatus Stage(UmiDocumentSourceBatch *batch, size_t index, const char *text)
{
    UmiDocumentSourceBatchSummary summary;
    UmiStatus status = UmiDocumentSourceBatchInspect(batch, &summary);
    return status == UMI_STATUS_OK
               ? UmiDocumentSourceBatchStage(batch, index, summary.revision, text, strlen(text), strlen(text))
               : status;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {
        "capture",     "ownership",  "apply",          "reverse-order",   "empty",
        "unchanged",   "mixed",      "pending-typing", "return-to-store", "unchanged-pending",
        "undo",        "redo",       "stale-first",    "stale-last",      "round-trip",
        "caret",       "selection",  "language",       "read-only",       "closed",
        "external",    "saved",      "other-owner",    "active-tab",      "presentation",
        "approval",    "revision",   "restage",        "incomplete",      "duplicate",
        "missing",     "invalid-id", "invalid-utf8",   "embedded-zero",   "cursor-utf8",
        "cursor-crlf", "large",      "limits",         "consumed",        "arguments"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    ReplacementFixture first = {0}, second = {0};
    CHECK(Start(&first, "first") == 0 && Second(&first, &second) == 0);
    const char *before_first = "first", *before_second = "second";
    char *large = NULL;
    if (strcmp(mode, "pending-typing") == 0 || strcmp(mode, "return-to-store") == 0 ||
        strcmp(mode, "unchanged-pending") == 0)
    {
        before_first = "first draft";
        before_second = "second draft";
        CHECK(Draft(&first, before_first) == UMI_STATUS_OK && Draft(&second, before_second) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "large") == 0)
    {
        large = malloc(40001U);
        CHECK(large != NULL);
        memset(large, 'x', 40000U);
        large[40000] = '\0';
        before_first = large;
        CHECK(Draft(&first, large) == UMI_STATUS_OK);
    }
    UmiDocumentId ids[2] = {first.id, second.id};
    size_t first_index = 0U, second_index = 1U;
    if (strcmp(mode, "reverse-order") == 0)
    {
        ids[0] = second.id;
        ids[1] = first.id;
        first_index = 1U;
        second_index = 0U;
    }
    UmiDocumentSourceBatch *batch = NULL;
    UmiStatus wanted = UMI_STATUS_OK;
    if (strcmp(mode, "duplicate") == 0)
    {
        ids[1] = ids[0];
        wanted = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(mode, "missing") == 0)
    {
        ids[1] = UINT64_MAX;
        wanted = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "invalid-id") == 0)
    {
        ids[1] = 0U;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    CHECK(UmiDocumentSourceBatchCreate(first.documents, ids, 2U, &batch) == wanted);
    if (wanted != UMI_STATUS_OK)
    {
        CHECK(batch == NULL && ExpectText(&first, before_first) == 0 &&
              ExpectText(&second, before_second) == 0);
        goto cleanup;
    }
    UmiDocumentSourceBatchSummary summary;
    CHECK(UmiDocumentSourceBatchInspect(batch, &summary) == UMI_STATUS_OK);
    CHECK(summary.document_count == 2U && summary.staged_count == 0U &&
          summary.source_bytes == strlen(before_first) + strlen(before_second));
    const char *captured = NULL;
    size_t bytes = 0U;
    CHECK(UmiDocumentSourceBatchRead(batch, first_index, &captured, &bytes) == UMI_STATUS_OK);
    CHECK(bytes == strlen(before_first) && strcmp(captured, before_first) == 0);
    if (strcmp(mode, "capture") == 0 || strcmp(mode, "ownership") == 0)
    {
        if (strcmp(mode, "ownership") == 0)
        {
            CHECK(Draft(&first, "later") == UMI_STATUS_OK);
            CHECK(strcmp(captured, before_first) == 0);
        }
        goto cleanup;
    }
    const char *after_first = "changed first", *after_second = "changed second";
    if (strcmp(mode, "empty") == 0)
        after_second = "";
    if (strcmp(mode, "unchanged") == 0 || strcmp(mode, "mixed") == 0 ||
        strcmp(mode, "unchanged-pending") == 0)
        after_first = before_first;
    if (strcmp(mode, "unchanged") == 0 || strcmp(mode, "unchanged-pending") == 0)
        after_second = before_second;
    if (strcmp(mode, "return-to-store") == 0)
    {
        after_first = "first";
        after_second = "second";
    }
    CHECK(Stage(batch, first_index, after_first) == UMI_STATUS_OK);
    if (strcmp(mode, "incomplete") != 0)
        CHECK(Stage(batch, second_index, after_second) == UMI_STATUS_OK);
    else
        wanted = UMI_STATUS_INVALID_STATE;
    CHECK(UmiDocumentSourceBatchInspect(batch, &summary) == UMI_STATUS_OK);
    uint64_t reviewed = summary.revision;
    int approved = 1;
    if (strcmp(mode, "approval") == 0)
    {
        approved = 0;
        wanted = UMI_STATUS_PERMISSION_DENIED;
    }
    if (strcmp(mode, "revision") == 0)
    {
        --reviewed;
        wanted = UMI_STATUS_BUSY;
    }
    if (strcmp(mode, "restage") == 0)
    {
        CHECK(Stage(batch, second_index, "restaged") == UMI_STATUS_OK);
        wanted = UMI_STATUS_BUSY;
    }
    if (strcmp(mode, "stale-first") == 0)
    {
        CHECK(Draft(&first, "later") == UMI_STATUS_OK);
        before_first = "later";
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "stale-last") == 0 || strcmp(mode, "round-trip") == 0)
    {
        CHECK(Draft(&second, "later") == UMI_STATUS_OK);
        before_second = "later";
        if (strcmp(mode, "round-trip") == 0)
        {
            before_second = "second";
            CHECK(Draft(&second, before_second) == UMI_STATUS_OK);
        }
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "caret") == 0 || strcmp(mode, "selection") == 0 || strcmp(mode, "language") == 0 ||
        strcmp(mode, "read-only") == 0 || strcmp(mode, "presentation") == 0)
    {
        UmiUiDocumentViewSnapshot view;
        UmiUiDocumentViewModel *views = umi_ui_workbench_documents(first.workbench);
        CHECK(umi_ui_document_view_model_find(views, second.viewId, &view) == UMI_STATUS_OK);
        if (strcmp(mode, "caret") == 0)
            view.cursor_offset = 1U;
        if (strcmp(mode, "selection") == 0)
            view.selection_length = 1U;
        if (strcmp(mode, "language") == 0)
            strcpy(view.language_id, "different");
        if (strcmp(mode, "read-only") == 0)
            view.read_only = 1;
        if (strcmp(mode, "presentation") == 0)
        {
            view.pinned = 1;
            view.word_wrap = 1;
            strcpy(view.title, "Keep this title");
        }
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
        if (strcmp(mode, "presentation") != 0)
            wanted = strcmp(mode, "read-only") == 0 ? UMI_STATUS_PERMISSION_DENIED : UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "closed") == 0)
    {
        CHECK(umi_document_coordinator_close_active(first.documents, 1) == UMI_STATUS_OK);
        wanted = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "external") == 0)
    {
        CHECK(umi_document_store_mark_external_change(first.store, second.id, 1) == UMI_STATUS_OK);
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "saved") == 0)
    {
        CHECK(umi_document_store_mark_saved_as(first.store, second.id, "second-saved.c") == UMI_STATUS_OK);
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "active-tab") == 0)
        CHECK(umi_ui_document_view_model_activate(umi_ui_workbench_documents(first.workbench),
                                                  first.viewId) == UMI_STATUS_OK);
    if (strcmp(mode, "other-owner") == 0)
    {
        ReplacementFixture other = {0};
        CHECK(Start(&other, "other") == 0);
        CHECK(UmiDocumentSourceBatchApply(other.documents, batch, reviewed, 1) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              ExpectText(&other, "other") == 0);
        Stop(&other);
        goto cleanup;
    }
    if (strcmp(mode, "invalid-utf8") == 0 || strcmp(mode, "embedded-zero") == 0 ||
        strcmp(mode, "cursor-utf8") == 0 || strcmp(mode, "cursor-crlf") == 0 || strcmp(mode, "limits") == 0)
    {
        const char *invalid = strcmp(mode, "invalid-utf8") == 0    ? "\xff"
                              : strcmp(mode, "embedded-zero") == 0 ? "a\0b"
                              : strcmp(mode, "cursor-utf8") == 0   ? "\xc3\xa9"
                                                                   : "a\r\nb";
        size_t length = strcmp(mode, "invalid-utf8") == 0    ? 1U
                        : strcmp(mode, "embedded-zero") == 0 ? 3U
                        : strcmp(mode, "cursor-utf8") == 0   ? 2U
                                                             : 4U;
        size_t cursor = strcmp(mode, "cursor-crlf") == 0 ? 2U : 1U;
        UmiStatus invalid_status = UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(mode, "limits") == 0)
        {
            length = UMI_DOCUMENT_SOURCE_BATCH_BYTE_BUDGET + 1U;
            invalid_status = UMI_STATUS_CAPACITY_EXCEEDED;
        }
        CHECK(UmiDocumentSourceBatchStage(batch, second_index, reviewed, invalid, length, cursor) ==
              invalid_status);
        UmiDocumentSourceBatchSummary current;
        CHECK(UmiDocumentSourceBatchInspect(batch, &current) == UMI_STATUS_OK &&
              current.revision == reviewed);
        const char *proposal = NULL;
        size_t proposal_bytes = 0U;
        CHECK(UmiDocumentSourceBatchProposed(batch, second_index, &proposal, &proposal_bytes) ==
                  UMI_STATUS_OK &&
              strcmp(proposal, after_second) == 0);
    }
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(first.documents, &active) == UMI_STATUS_OK);
    uint64_t view_revision = umi_ui_document_view_model_revision(umi_ui_workbench_documents(first.workbench));
    UmiDocumentSnapshot store_before;
    CHECK(umi_document_store_snapshot(first.store, first.id, &store_before) == UMI_STATUS_OK);
    CHECK(UmiDocumentSourceBatchApply(first.documents, batch, reviewed, approved) == wanted);
    if (wanted != UMI_STATUS_OK)
    {
        CHECK(ExpectText(&first, before_first) == 0);
        if (strcmp(mode, "closed") != 0)
            CHECK(ExpectText(&second, before_second) == 0);
        CHECK(umi_ui_document_view_model_revision(umi_ui_workbench_documents(first.workbench)) ==
              view_revision);
        UmiDocumentSnapshot store_after;
        CHECK(umi_document_store_snapshot(first.store, first.id, &store_after) == UMI_STATUS_OK &&
              store_after.revision == store_before.revision);
        if (strcmp(mode, "restage") == 0)
        {
            CHECK(UmiDocumentSourceBatchInspect(batch, &summary) == UMI_STATUS_OK);
            CHECK(UmiDocumentSourceBatchApply(first.documents, batch, summary.revision, 1) == UMI_STATUS_OK &&
                  ExpectText(&second, "restaged") == 0);
        }
        goto cleanup;
    }
    CHECK(ExpectText(&first, after_first) == 0 && ExpectText(&second, after_second) == 0);
    UmiDocumentWorkingCopySnapshot after_active;
    CHECK(umi_document_coordinator_active_snapshot(first.documents, &after_active) == UMI_STATUS_OK &&
          after_active.document_id == active.document_id);
    CHECK(UmiDocumentSourceBatchInspect(batch, &summary) == UMI_STATUS_OK && summary.applied);
    if (strcmp(mode, "return-to-store") == 0)
    {
        UmiDocumentSnapshot current;
        CHECK(umi_document_store_snapshot(first.store, first.id, &current) == UMI_STATUS_OK &&
              current.revision == store_before.revision + 1U);
    }
    if (strcmp(mode, "unchanged-pending") == 0)
    {
        UmiDocumentSnapshot current;
        CHECK(umi_document_store_snapshot(first.store, first.id, &current) == UMI_STATUS_OK &&
              current.revision == store_before.revision);
    }
    if (strcmp(mode, "undo") == 0 || strcmp(mode, "redo") == 0 || strcmp(mode, "pending-typing") == 0 ||
        strcmp(mode, "return-to-store") == 0 || strcmp(mode, "large") == 0)
    {
        CHECK(UmiDocumentCoordinatorUndo(first.documents, first.id) == UMI_STATUS_OK &&
              ExpectText(&first, before_first) == 0);
        CHECK(ExpectText(&second, after_second) == 0);
        CHECK(UmiDocumentCoordinatorUndo(first.documents, second.id) == UMI_STATUS_OK &&
              ExpectText(&second, before_second) == 0);
        if (strcmp(mode, "redo") == 0)
        {
            CHECK(UmiDocumentCoordinatorRedo(first.documents, first.id) == UMI_STATUS_OK &&
                  ExpectText(&first, after_first) == 0);
        }
        if (strcmp(mode, "pending-typing") == 0 || strcmp(mode, "return-to-store") == 0)
        {
            CHECK(UmiDocumentCoordinatorUndo(first.documents, first.id) == UMI_STATUS_OK &&
                  ExpectText(&first, "first") == 0);
        }
    }
    if (strcmp(mode, "presentation") == 0)
    {
        UmiUiDocumentViewSnapshot view;
        CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(first.workbench), second.viewId,
                                              &view) == UMI_STATUS_OK &&
              view.pinned && view.word_wrap && strcmp(view.title, "Keep this title") == 0);
    }
    if (strcmp(mode, "consumed") == 0)
    {
        CHECK(UmiDocumentSourceBatchApply(first.documents, batch, reviewed, 1) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentSourceBatchStage(batch, 0U, reviewed, "again", 5U, 0U) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentSourceBatchCheck(first.documents, batch) == UMI_STATUS_INVALID_STATE);
    }
    if (strcmp(mode, "arguments") == 0)
    {
        UmiDocumentSourceBatch *other = (UmiDocumentSourceBatch *)1;
        CHECK(UmiDocumentSourceBatchCreate(NULL, ids, 2U, &other) == UMI_STATUS_INVALID_ARGUMENT &&
              other == NULL);
        CHECK(UmiDocumentSourceBatchCreate(first.documents, ids, 0U, &other) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentSourceBatchCreate(first.documents, ids, UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM + 1U,
                                           &other) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiDocumentSourceBatchAt(batch, 2U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        UmiDocumentSourceRequestSummary member;
        CHECK(UmiDocumentSourceBatchAt(batch, 2U, &member) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiDocumentSourceBatchRead(batch, 2U, &captured, &bytes) == UMI_STATUS_NOT_FOUND &&
              captured == NULL && bytes == 0U);
        UmiDocumentSourceBatchDestroy(NULL);
    }
cleanup:
    UmiDocumentSourceBatchDestroy(batch);
    free(large);
    Stop(&first);
    return 0;
}
