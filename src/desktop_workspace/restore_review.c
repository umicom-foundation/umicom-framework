/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_workspace/restore_review.c
 * PURPOSE: Apply the exact reviewed checkpoint through the existing workspace transaction.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop_workspace/restore_review.h"
#include "internal.h"

/* The service owns the large snapshots on the heap. Native frontends can
 * display every note without borrowing storage buffers or copying them onto
 * a small Windows worker stack. The owner pointer is an identity, not a ref. */
struct UmiDesktopWorkspaceRestoreReview {
    UmiDesktopWorkspace *owner;
    UmiDesktopWorkspaceSnapshot current, checkpoint;
    int applied;
};

UmiStatus UmiDesktopWorkspacePrepareRestore(UmiDesktopWorkspace *workspace,
    uint64_t checkpointRevision, UmiDesktopWorkspaceRestoreReview **outReview)
{
    if (!workspace || !outReview || !checkpointRevision) return UMI_STATUS_INVALID_ARGUMENT;
    /* Subtract addresses rather than adding sizes, which could wrap near the
     * end of the address space. Reject publication into the borrowed owner. */
    const uintptr_t owner = (uintptr_t)workspace, output = (uintptr_t)outReview;
    if (owner <= output ? output - owner < sizeof(*workspace) : owner - output < sizeof(*outReview))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->closed || workspace->poisoned) return UMI_STATUS_INVALID_STATE;
    UmiDesktopWorkspaceRestoreReview *review = calloc(1U, sizeof(*review));
    if (!review) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiDesktopWorkspaceRead(workspace, &review->current);
    if (status == UMI_STATUS_OK)
        status = UmiDesktopWorkspaceReadCheckpoint(workspace, checkpointRevision, &review->checkpoint);
    if (status != UMI_STATUS_OK) { free(review); return status; }
    review->owner = workspace; *outReview = review;
    return UMI_STATUS_OK;
}

const UmiDesktopWorkspaceSnapshot *UmiDesktopWorkspaceRestoreCurrent(
    const UmiDesktopWorkspaceRestoreReview *review)
{ return review ? &review->current : NULL; }

const UmiDesktopWorkspaceSnapshot *UmiDesktopWorkspaceRestoreCheckpoint(
    const UmiDesktopWorkspaceRestoreReview *review)
{ return review ? &review->checkpoint : NULL; }

UmiStatus UmiDesktopWorkspaceApplyRestore(UmiDesktopWorkspace *workspace,
    UmiDesktopWorkspaceRestoreReview *review)
{
    if (!workspace || !review) return UMI_STATUS_INVALID_ARGUMENT;
    if (review->owner != workspace || review->applied || workspace->closed || workspace->poisoned ||
        workspace->snapshot.revision != review->current.revision) return UMI_STATUS_INVALID_STATE;
    UmiDesktopWorkspaceSnapshot *draft = malloc(sizeof(*draft));
    if (!draft) return UMI_STATUS_OUT_OF_MEMORY;
    /* Commit expects the current revision on a draft. Keep the original
     * checkpoint evidence intact so callers can still explain the restore. */
    *draft = review->checkpoint; draft->revision = review->current.revision;
    const UmiStatus status = UmiDesktopWorkspaceCommit(workspace, review->current.revision, draft);
    free(draft);
    if (status == UMI_STATUS_OK) review->applied = 1;
    return status;
}

void UmiDesktopWorkspaceDestroyRestoreReview(UmiDesktopWorkspaceRestoreReview *review)
{ free(review); }
