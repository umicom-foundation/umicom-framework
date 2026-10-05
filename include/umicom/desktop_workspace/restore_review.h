/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop_workspace/restore_review.h
 * PURPOSE: Freeze complete desktop notes and preferences for an explicit restore review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_WORKSPACE_RESTORE_REVIEW_H
#define UMICOM_DESKTOP_WORKSPACE_RESTORE_REVIEW_H
#include "umicom/desktop_workspace/workspace.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesktopWorkspaceRestoreReview UmiDesktopWorkspaceRestoreReview;

/** Capture the current saved workspace and one retained checkpoint together.
 * No draft, checkpoint or session marker is changed. Both copies belong to
 * the review; reading them performs no storage access. Failed preparation
 * leaves outReview unchanged. Its storage must not overlap the workspace.
 * Serialise calls with the workspace's sole worker, as for Commit. */
UmiStatus UmiDesktopWorkspacePrepareRestore(UmiDesktopWorkspace *workspace,
    uint64_t checkpointRevision, UmiDesktopWorkspaceRestoreReview **outReview);

/** Borrow complete immutable evidence until DestroyRestoreReview. NULL input
 * returns NULL. The checkpoint retains its original revision for display. */
const UmiDesktopWorkspaceSnapshot *UmiDesktopWorkspaceRestoreCurrent(
    const UmiDesktopWorkspaceRestoreReview *review);
const UmiDesktopWorkspaceSnapshot *UmiDesktopWorkspaceRestoreCheckpoint(
    const UmiDesktopWorkspaceRestoreReview *review);

/** Commit the exact reviewed checkpoint as a new revision. The original
 * workspace must remain alive and its saved revision must still match the
 * review. A different owner, closed session, stale or already applied review
 * is refused. No checkpoint is reread. Existing transaction, digest and head
 * checks still protect the commit. Success consumes the review's permission
 * to apply, but its evidence remains readable. Failure does not consume it.
 * Unsaved editor drafts are the host's responsibility: ask the user to save
 * or discard them before preparing a review. */
UmiStatus UmiDesktopWorkspaceApplyRestore(UmiDesktopWorkspace *workspace,
    UmiDesktopWorkspaceRestoreReview *review);
/** Releases only copied evidence; it does not access the borrowed owner. */
void UmiDesktopWorkspaceDestroyRestoreReview(UmiDesktopWorkspaceRestoreReview *review);

#ifdef __cplusplus
}
#endif
#endif
