/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_workspace/test_restore_review.c
 * PURPOSE: Exercise frozen restore evidence, owner identity and failure-atomic commits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop_workspace/restore_review.h"
#include "umicom/test_runtime/check.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CHECK(value) UMI_TEST_REQUIRE(value)
#define OK(value) CHECK((value) == UMI_STATUS_OK)

/* All scenarios use the real memory Data Server. Mutation of its records is
 * deliberate failure injection; application code uses the workspace API. */
int main(int argc, char **argv)
{
    const char *cases[] = {"evidence", "apply", "stale", "foreign", "consumed", "closed",
        "invalid", "retired", "dismiss", "frozen-record", "external-head", "retry", "capacity"};
    if (argc != 2) return 2;
    int known = 0;
    for (size_t index = 0U; index < sizeof cases / sizeof cases[0]; ++index)
        if (!strcmp(argv[1], cases[index])) known = 1;
    if (!known) return 2;
    UmiDataServer *server = NULL;
    UmiDesktopWorkspace *workspace = NULL, *other = NULL;
    UmiDesktopWorkspaceRestoreReview *review = NULL;
    UmiDesktopWorkspaceSnapshot *draft = calloc(1U, sizeof(*draft));
    CHECK(draft != NULL);
    OK(umi_data_server_create_memory(&server)); OK(UmiDesktopWorkspaceOpenServer(server, &workspace));
    OK(UmiDesktopWorkspaceRead(workspace, draft));
    OK(UmiDesktopWorkspacePutNote(draft, "first", "Research café", "First line\n日本語\n"));
    OK(UmiDesktopWorkspacePutNote(draft, "second", "Plan", "Keep this complete body."));
    draft->theme = UMI_DESKTOP_WORKSPACE_DARK; draft->fontPoints = 17U; draft->sidebarVisible = 0;
    if (!strcmp(argv[1], "capacity")) {
        char body[UMI_DESKTOP_WORKSPACE_BODY]; memset(body, 'x', sizeof body - 1U); body[sizeof body - 1U] = '\0';
        for (size_t index = 0U; index < UMI_DESKTOP_WORKSPACE_NOTES; ++index) {
            char id[32]; (void)snprintf(id, sizeof id, "note-%zu", index);
            if (index == 0U) strcpy(id, "first");
            if (index == 1U) strcpy(id, "second");
            OK(UmiDesktopWorkspacePutNote(draft, id, "Complete note", body));
        }
    }
    OK(UmiDesktopWorkspaceCommit(workspace, draft->revision, draft));
    OK(UmiDesktopWorkspaceRead(workspace, draft));
    const uint64_t checkpoint = draft->revision;
    OK(UmiDesktopWorkspacePutNote(draft, "first", "Changed title", "Newer saved body"));
    draft->fontPoints = 20U;
    OK(UmiDesktopWorkspaceCommit(workspace, draft->revision, draft));
    OK(UmiDesktopWorkspaceRead(workspace, draft));
    const uint64_t current = draft->revision;
    OK(UmiDesktopWorkspacePrepareRestore(workspace, checkpoint, &review));
    const UmiDesktopWorkspaceSnapshot *before = UmiDesktopWorkspaceRestoreCurrent(review);
    const UmiDesktopWorkspaceSnapshot *after = UmiDesktopWorkspaceRestoreCheckpoint(review);
    CHECK(before && after && before->revision == current && after->revision == checkpoint);
    CHECK(before->fontPoints == 20U && after->fontPoints == 17U);
    CHECK(!strcmp(before->notes[0].body, "Newer saved body"));
    CHECK(after->theme == UMI_DESKTOP_WORKSPACE_DARK && !after->sidebarVisible);

    if (!strcmp(argv[1], "evidence")) {
        CHECK(!strcmp(after->notes[0].body, "First line\n日本語\n"));
        OK(UmiDesktopWorkspacePutNote(draft, "first", "Unsaved", "Caller draft"));
        CHECK(!strcmp(before->notes[0].title, "Changed title"));
        OK(UmiDesktopWorkspaceRead(workspace, draft)); CHECK(draft->revision == current);
    } else if (!strcmp(argv[1], "invalid")) {
        UmiDesktopWorkspaceRestoreReview *same = review;
        CHECK(UmiDesktopWorkspacePrepareRestore(NULL, checkpoint, &review) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDesktopWorkspacePrepareRestore(workspace, 0U, &review) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDesktopWorkspacePrepareRestore(workspace, UINT64_MAX, &review) == UMI_STATUS_NOT_FOUND);
        CHECK(review == same && UmiDesktopWorkspaceRestoreCurrent(NULL) == NULL);
        CHECK(UmiDesktopWorkspaceRestoreCheckpoint(NULL) == NULL);
        CHECK(UmiDesktopWorkspaceApplyRestore(workspace, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        UmiDesktopWorkspaceDestroyRestoreReview(NULL);
    } else if (!strcmp(argv[1], "foreign")) {
        OK(UmiDesktopWorkspaceOpenMemory(&other));
        CHECK(UmiDesktopWorkspaceApplyRestore(other, review) == UMI_STATUS_INVALID_STATE);
        OK(UmiDesktopWorkspaceApplyRestore(workspace, review));
    } else if (!strcmp(argv[1], "closed")) {
        OK(UmiDesktopWorkspaceCloseClean(workspace));
        CHECK(UmiDesktopWorkspaceApplyRestore(workspace, review) == UMI_STATUS_INVALID_STATE);
        UmiDesktopWorkspaceRestoreReview *same = review;
        CHECK(UmiDesktopWorkspacePrepareRestore(workspace, checkpoint, &review) == UMI_STATUS_INVALID_STATE);
        CHECK(same == review);
    } else if (!strcmp(argv[1], "stale") || !strcmp(argv[1], "retired")) {
        size_t count = !strcmp(argv[1], "retired") ? UMI_DESKTOP_WORKSPACE_HISTORY : 1U;
        for (size_t index = 0U; index < count; ++index) {
            OK(UmiDesktopWorkspaceRead(workspace, draft));
            OK(UmiDesktopWorkspaceCommit(workspace, draft->revision, draft));
        }
        CHECK(UmiDesktopWorkspaceApplyRestore(workspace, review) == UMI_STATUS_INVALID_STATE);
        CHECK(after->revision == checkpoint && after->fontPoints == 17U);
        OK(UmiDesktopWorkspaceRead(workspace, draft)); CHECK(draft->revision == current + count);
    } else if (!strcmp(argv[1], "dismiss")) {
        UmiDesktopWorkspaceDestroyRestoreReview(review); review = NULL;
        OK(UmiDesktopWorkspaceRead(workspace, draft)); CHECK(draft->revision == current);
    } else if (!strcmp(argv[1], "external-head")) {
        OK(umi_data_server_set(server, "desktop.workspace.head", "foreign head"));
        CHECK(UmiDesktopWorkspaceApplyRestore(workspace, review) == UMI_STATUS_INVALID_STATE);
        OK(UmiDesktopWorkspaceRead(workspace, draft)); CHECK(draft->revision == current);
        CHECK(!umi_data_server_in_transaction(server));
    } else {
        if (!strcmp(argv[1], "frozen-record")) {
            /* A corrupted old record cannot replace the copied evidence. The
             * head stays unchanged; normal Commit still guards publication. */
            OK(umi_data_server_set(server, "desktop.workspace.g.00000000000000000002.000", "changed after review"));
            CHECK(UmiDesktopWorkspaceReadCheckpoint(workspace, checkpoint, draft) != UMI_STATUS_OK);
        }
        if (!strcmp(argv[1], "retry")) {
            OK(umi_data_server_begin(server));
            CHECK(UmiDesktopWorkspaceApplyRestore(workspace, review) == UMI_STATUS_BUSY);
            CHECK(umi_data_server_in_transaction(server)); OK(umi_data_server_rollback(server));
        }
        OK(UmiDesktopWorkspaceApplyRestore(workspace, review));
        OK(UmiDesktopWorkspaceRead(workspace, draft));
        CHECK(draft->revision == current + 1U && draft->fontPoints == 17U);
        CHECK(!strcmp(draft->notes[0].body, after->notes[0].body));
        CHECK(!strcmp(draft->selectedNote, after->selectedNote));
        CHECK(after->revision == checkpoint && before->revision == current);
        if (!strcmp(argv[1], "capacity")) CHECK(draft->noteCount == UMI_DESKTOP_WORKSPACE_NOTES &&
            strlen(draft->notes[0].body) == UMI_DESKTOP_WORKSPACE_BODY - 1U);
        if (!strcmp(argv[1], "consumed")) {
            CHECK(UmiDesktopWorkspaceApplyRestore(workspace, review) == UMI_STATUS_INVALID_STATE);
            OK(UmiDesktopWorkspaceRead(workspace, draft)); CHECK(draft->revision == current + 1U);
        }
    }
    UmiDesktopWorkspaceDestroy(other); UmiDesktopWorkspaceDestroy(workspace);
    /* Evidence destruction never dereferences its now-destroyed owner. */
    UmiDesktopWorkspaceDestroyRestoreReview(review); umi_data_server_destroy(server); free(draft);
    return EXIT_SUCCESS;
}
