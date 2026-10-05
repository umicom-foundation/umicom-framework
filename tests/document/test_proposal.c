/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_proposal.c
 * PURPOSE: Exercise captured source edits, immutable comparison, stale revisions and the real Undo transaction.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/proposal.h"
static UmiStatus Select(ReplacementFixture *f, size_t offset, size_t count)
{
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f->workbench);
    UmiUiDocumentViewSnapshot view;
    UmiStatus status = umi_ui_document_view_model_find(views, f->viewId, &view);
    if (status == UMI_STATUS_OK)
    {
        view.cursor_offset = offset;
        view.selection_length = count;
        status = umi_ui_document_view_model_upsert(views, &view);
    }
    return status;
}
static int Texts(UmiDocumentProposal *proposal, const char *before, const char *after)
{
    const char *left = NULL, *right = NULL;
    size_t left_bytes = 0U, right_bytes = 0U;
    CHECK(UmiDocumentProposalTexts(proposal, &left, &left_bytes, &right, &right_bytes) == UMI_STATUS_OK);
    CHECK(left_bytes == strlen(before) && right_bytes == strlen(after) && strcmp(left, before) == 0 &&
          strcmp(right, after) == 0);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    const char *cases[] = {"capture",    "apply",     "pending-typing", "owned",       "delete",
                           "unchanged",  "approval",  "revision",       "invalid",     "stale",
                           "round-trip", "read-only", "closed",         "other-owner", "other-tab",
                           "caret",      "unicode",   "large",          "limits",      "arguments"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    if (!known)
        return 2;
    ReplacementFixture f = {0};
    CHECK(Start(&f, "before old after") == 0);
    CHECK(Select(&f, 7U, 3U) == UMI_STATUS_OK);
    if (strcmp(name, "pending-typing") == 0)
    {
        CHECK(Draft(&f, "before old pending") == UMI_STATUS_OK);
        CHECK(Select(&f, 7U, 3U) == UMI_STATUS_OK);
    }
    UmiDocumentProposal *proposal = NULL;
    CHECK(UmiDocumentProposalCreate(f.documents, f.id, &proposal) == UMI_STATUS_OK);
    UmiDocumentProposalSummary summary;
    CHECK(UmiDocumentProposalInspect(proposal, &summary) == UMI_STATUS_OK);
    CHECK(summary.selection_offset == 7U && summary.selection_bytes == 3U && summary.revision == 1U &&
          !summary.has_proposal);
    const char *selected = NULL;
    size_t bytes = 0U;
    CHECK(UmiDocumentProposalSelection(proposal, &selected, &bytes) == UMI_STATUS_OK && bytes == 3U &&
          memcmp(selected, "old", 3U) == 0);
    char replacement[] = "new";
    CHECK(UmiDocumentProposalSetText(proposal, summary.revision, replacement, 3U) == UMI_STATUS_OK);
    CHECK(UmiDocumentProposalInspect(proposal, &summary) == UMI_STATUS_OK && summary.revision == 2U &&
          summary.has_proposal);
    if (strcmp(name, "capture") == 0)
    {
        CHECK(Texts(proposal, "before old after", "before new after") == 0);
        CHECK(ExpectText(&f, "before old after") == 0 && summary.document_id == f.id && summary.text_changes);
    }
    else if (strcmp(name, "apply") == 0 || strcmp(name, "pending-typing") == 0)
    {
        int pending = strcmp(name, "pending-typing") == 0;
        CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 1) == UMI_STATUS_OK);
        CHECK(ExpectText(&f, pending ? "before new pending" : "before new after") == 0);
        CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK);
        CHECK(ExpectText(&f, pending ? "before old pending" : "before old after") == 0);
        if (pending)
        {
            CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK);
            CHECK(ExpectText(&f, "before old after") == 0);
        }
        CHECK(UmiDocumentProposalInspect(proposal, &summary) == UMI_STATUS_OK && summary.applied);
        CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 1) ==
              UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentProposalSetText(proposal, summary.revision, "more", 4U) == UMI_STATUS_INVALID_STATE);
    }
    else if (strcmp(name, "owned") == 0)
    {
        replacement[0] = '!';
        CHECK(Texts(proposal, "before old after", "before new after") == 0);
        CHECK(Draft(&f, "later source") == UMI_STATUS_OK);
        CHECK(Texts(proposal, "before old after", "before new after") == 0);
        CHECK(UmiDocumentProposalCheck(f.documents, proposal) == UMI_STATUS_INVALID_STATE);
    }
    else if (strcmp(name, "delete") == 0 || strcmp(name, "unchanged") == 0)
    {
        int same = strcmp(name, "unchanged") == 0;
        CHECK(UmiDocumentProposalSetText(proposal, summary.revision, same ? "old" : "", same ? 3U : 0U) ==
              UMI_STATUS_OK);
        CHECK(UmiDocumentProposalInspect(proposal, &summary) == UMI_STATUS_OK &&
              summary.text_changes == !same);
        UmiDocumentWorkingCopySnapshot before, after;
        CHECK(umi_document_coordinator_active_snapshot(f.documents, &before) == UMI_STATUS_OK);
        CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 1) == UMI_STATUS_OK);
        CHECK(ExpectText(&f, same ? "before old after" : "before  after") == 0);
        if (same)
        {
            CHECK(umi_document_coordinator_active_snapshot(f.documents, &after) == UMI_STATUS_OK);
            CHECK(after.undo_count == before.undo_count && after.revision == before.revision);
        }
        else
        {
            CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK);
            CHECK(ExpectText(&f, "before old after") == 0);
        }
    }
    else if (strcmp(name, "approval") == 0)
    {
        CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 0) ==
              UMI_STATUS_PERMISSION_DENIED);
        CHECK(ExpectText(&f, "before old after") == 0 &&
              UmiDocumentProposalCheck(f.documents, proposal) == UMI_STATUS_OK);
    }
    else if (strcmp(name, "revision") == 0)
    {
        CHECK(UmiDocumentProposalSetText(proposal, summary.revision - 1U, "bad", 3U) == UMI_STATUS_BUSY);
        CHECK(UmiDocumentProposalSetText(proposal, summary.revision, "later", 5U) == UMI_STATUS_OK);
        CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 1) == UMI_STATUS_BUSY);
        CHECK(Texts(proposal, "before old after", "before later after") == 0);
    }
    else if (strcmp(name, "invalid") == 0)
    {
        const char nul[] = {'a', '\0', 'b'};
        CHECK(UmiDocumentProposalSetText(proposal, summary.revision, nul, sizeof(nul)) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentProposalSetText(proposal, summary.revision, "\xc0\xaf", 2U) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(Texts(proposal, "before old after", "before new after") == 0);
        CHECK(UmiDocumentProposalInspect(proposal, &summary) == UMI_STATUS_OK && summary.revision == 2U);
    }
    else if (strcmp(name, "stale") == 0 || strcmp(name, "round-trip") == 0)
    {
        CHECK(Draft(&f, "later source") == UMI_STATUS_OK);
        if (strcmp(name, "round-trip") == 0)
        {
            CHECK(Draft(&f, "before old after") == UMI_STATUS_OK);
            CHECK(Select(&f, 7U, 3U) == UMI_STATUS_OK);
        }
        CHECK(UmiDocumentProposalCheck(f.documents, proposal) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 1) ==
              UMI_STATUS_INVALID_STATE);
    }
    else if (strcmp(name, "read-only") == 0)
    {
        UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
        UmiUiDocumentViewSnapshot view;
        CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
        view.read_only = 1;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
        CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 1) ==
              UMI_STATUS_PERMISSION_DENIED);
        UmiDocumentProposal *denied = NULL;
        CHECK(UmiDocumentProposalCreate(f.documents, f.id, &denied) == UMI_STATUS_PERMISSION_DENIED &&
              denied == NULL);
    }
    else if (strcmp(name, "closed") == 0)
    {
        CHECK(UmiDocumentCoordinatorClose(f.documents, f.id, 1) == UMI_STATUS_OK);
        CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 1) == UMI_STATUS_NOT_FOUND);
    }
    else if (strcmp(name, "other-owner") == 0)
    {
        ReplacementFixture other = {0};
        CHECK(Start(&other, "before old after") == 0);
        CHECK(UmiDocumentProposalApply(other.documents, proposal, summary.revision, 1) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ExpectText(&other, "before old after") == 0);
        Stop(&other);
    }
    else if (strcmp(name, "other-tab") == 0)
    {
        char other_view[UMI_UI_ID_CAPACITY];
        CHECK(umi_document_coordinator_new(f.documents, "other.c", other_view, sizeof(other_view)) ==
              UMI_STATUS_OK);
        CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 1) == UMI_STATUS_OK);
        CHECK(ExpectText(&f, "before new after") == 0);
        UmiDocumentWorkingCopySnapshot active;
        CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK);
        CHECK(strcmp(active.view_id, other_view) == 0);
    }
    else if (strcmp(name, "caret") == 0)
    {
        CHECK(Select(&f, 0U, 1U) == UMI_STATUS_OK);
        CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 1) == UMI_STATUS_OK);
        CHECK(ExpectText(&f, "before new after") == 0);
    }
    else if (strcmp(name, "unicode") == 0)
    {
        UmiDocumentProposalDestroy(proposal);
        proposal = NULL;
        CHECK(Draft(&f, "a caf\xc3\xa9 z") == UMI_STATUS_OK);
        CHECK(Select(&f, 5U, 2U) == UMI_STATUS_OK);
        CHECK(UmiDocumentProposalCreate(f.documents, f.id, &proposal) == UMI_STATUS_OK);
        CHECK(UmiDocumentProposalSetText(proposal, 1U, "\xce\xbb", 2U) == UMI_STATUS_OK);
        CHECK(Texts(proposal, "a caf\xc3\xa9 z", "a caf\xce\xbb z") == 0);
        CHECK(UmiDocumentProposalApply(f.documents, proposal, 2U, 1) == UMI_STATUS_OK);
        CHECK(ExpectText(&f, "a caf\xce\xbb z") == 0);
    }
    else if (strcmp(name, "large") == 0)
    {
        size_t length = 131072U;
        char *text = malloc(length + 1U);
        CHECK(text != NULL);
        memset(text, 'a', length);
        text[length] = '\0';
        memcpy(text + length - 6U, "target", 6U);
        UmiDocumentProposalDestroy(proposal);
        proposal = NULL;
        CHECK(Draft(&f, text) == UMI_STATUS_OK);
        CHECK(Select(&f, length - 6U, 6U) == UMI_STATUS_OK);
        CHECK(UmiDocumentProposalCreate(f.documents, f.id, &proposal) == UMI_STATUS_OK);
        CHECK(UmiDocumentProposalSetText(proposal, 1U, "changed", 7U) == UMI_STATUS_OK);
        CHECK(UmiDocumentProposalApply(f.documents, proposal, 2U, 1) == UMI_STATUS_OK);
        char *current = NULL;
        size_t current_bytes = 0U;
        CHECK(UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(f.workbench), f.viewId, &current,
                                             &current_bytes) == UMI_STATUS_OK);
        CHECK(current_bytes == length + 1U && memcmp(current, text, length - 6U) == 0 &&
              strcmp(current + length - 6U, "changed") == 0);
        UmiUiDocumentViewModelFreeText(current);
        free(text);
    }
    else if (strcmp(name, "limits") == 0)
    {
        CHECK(UmiDocumentProposalSetText(proposal, summary.revision, "x", SIZE_MAX) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(Texts(proposal, "before old after", "before new after") == 0);
    }
    else if (strcmp(name, "arguments") == 0)
    {
        UmiDocumentProposal *empty = NULL;
        CHECK(UmiDocumentProposalCreate(NULL, f.id, &empty) == UMI_STATUS_INVALID_ARGUMENT && empty == NULL);
        CHECK(UmiDocumentProposalCreate(f.documents, f.id, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentProposalInspect(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentProposalSelection(NULL, &selected, &bytes) == UMI_STATUS_INVALID_ARGUMENT &&
              selected == NULL && bytes == 0U);
        CHECK(Select(&f, 0U, 0U) == UMI_STATUS_OK);
        CHECK(UmiDocumentProposalCreate(f.documents, f.id, &empty) == UMI_STATUS_INVALID_ARGUMENT &&
              empty == NULL);
        UmiDocumentProposalDestroy(NULL);
    }
    UmiDocumentProposalDestroy(proposal);
    Stop(&f);
    return 0;
}
