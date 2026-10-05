/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/desktop_workspace/review_restore.c
 * PURPOSE: Teach preparation, inspection and explicit application of a frozen checkpoint.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop_workspace/restore_review.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

/* This offline example uses memory storage. A GUI can display the same
 * borrowed evidence while holding the review, then apply only after the user
 * confirms it. Real applications keep their workspace owner alive throughout. */
int main(void)
{
    UmiDesktopWorkspace *workspace = NULL;
    UmiDesktopWorkspaceRestoreReview *review = NULL;
    UmiDesktopWorkspaceSnapshot *draft = malloc(sizeof(*draft));
    UmiStatus status = draft ? UmiDesktopWorkspaceOpenMemory(&workspace) : UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceRead(workspace, draft);
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspacePutNote(draft, "plan", "Workshop plan", "Prepare the first exercise.");
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceCommit(workspace, draft->revision, draft);
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceRead(workspace, draft);
    const uint64_t original = status == UMI_STATUS_OK ? draft->revision : 0U;
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspacePutNote(draft, "plan", "Workshop plan", "Prepare the first and second exercises.");
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceCommit(workspace, draft->revision, draft);
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspacePrepareRestore(workspace, original, &review);
    if (status == UMI_STATUS_OK) {
        const UmiDesktopWorkspaceSnapshot *current = UmiDesktopWorkspaceRestoreCurrent(review);
        const UmiDesktopWorkspaceSnapshot *checkpoint = UmiDesktopWorkspaceRestoreCheckpoint(review);
        printf("Current: %s\nReviewed: %s\n", current->notes[0].body, checkpoint->notes[0].body);
        /* This scripted lesson explicitly elects to restore the earlier text.
         * Preparing or displaying the evidence alone would change nothing. */
        status = UmiDesktopWorkspaceApplyRestore(workspace, review);
    }
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceRead(workspace, draft);
    if (status == UMI_STATUS_OK) printf("Saved revision %" PRIu64 ": %s\n", draft->revision, draft->notes[0].body);
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceCloseClean(workspace);
    UmiDesktopWorkspaceDestroyRestoreReview(review); UmiDesktopWorkspaceDestroy(workspace); free(draft);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Workspace operation stopped with status %d.\n", (int)status);
    return status == UMI_STATUS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
