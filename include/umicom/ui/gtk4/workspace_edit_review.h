/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workspace_edit_review.h
 * PURPOSE: Present a complete source-edit transaction with document and annotation approval.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSPACE_EDIT_REVIEW_H
#define UMICOM_UI_GTK4_WORKSPACE_EDIT_REVIEW_H
#include "umicom/ui/gtk4/document_commands.h"
#include "umicom/source_review/workspace_edit.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Present the complete model on the bound document owner's GTK thread. A
 * different coordinator, stale input or busy document workflow fails preflight
 * without consuming *review. Once construction starts, the window consumes it
 * and clears *review, including if parent closure cancels presentation.
 *
 * Each changed draft has a full comparison and a separate review action.
 * Referenced confirmation annotations are displayed as literal text with
 * individual checkboxes. Final approval applies all drafts without saving.
 * Closing, parent closure, unbinding and CancelReplacementReview retire the
 * controls. Existing sequential and complete literal replacement reviews stay
 * available. Use the same bound adapter throughout this call. */
    UmiStatus UmiGtk4AdapterReviewWorkspaceEdits(UmiGtk4Adapter *adapter,
                                                 UmiSourceWorkspaceEditReview **review);
#ifdef __cplusplus
}
#endif
#endif
