/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/working_tree_review.h
 * PURPOSE: Explain staged changes and child repository ownership for source-control clients.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_WORKING_TREE_REVIEW_H
#define UMICOM_VCS_WORKING_TREE_REVIEW_H
#include "umicom/vcs/working_tree.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_VCS_WORKING_TREE_SUMMARY_TEXT_CAPACITY 4096U
#define UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY (UMI_VCS_PATH_CAPACITY * 8U + 1024U)
    /**
     * Format an educational explanation from one immutable observation, without changing Git.
     * Control characters in names are escaped. Failure leaves the caller's buffer unchanged.
     */
    UmiStatus UmiVcsWorkingTreeSummaryText(const UmiVcsWorkingTree *tree, char *output,
                                           size_t capacity);
    /**
     * Explain one path's index, worktree and submodule states. Escape control characters for
     * display; clients must use the original entry path for operations, never this formatted label.
     * Failure preserves output, including when the supplied buffer is too small.
     */
    UmiStatus UmiVcsWorkingTreeEntryText(const UmiVcsWorkingTree *tree, size_t index, char *output,
                                         size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
