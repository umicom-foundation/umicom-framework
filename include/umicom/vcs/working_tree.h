/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/working_tree.h
 * PURPOSE: Observe Git working trees and submodule changes without mutating repository state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_WORKING_TREE_H
#define UMICOM_VCS_WORKING_TREE_H

#include "umicom/base/status.h"
#include "umicom/platform/process.h"
#include "umicom/vcs/change.h"
#include <stddef.h>
#include <stdint.h>

/* Limit both input storage and result records before accepting external Git output. */
#define UMI_VCS_WORKING_TREE_BYTE_LIMIT (32U * 1024U * 1024U)

#ifdef __cplusplus
extern "C"
{
#endif

    /** Own an immutable observation. Destroy it after all borrowed entries are finished. */
    typedef struct UmiVcsWorkingTree UmiVcsWorkingTree;

    /** Preserve Git's independent index, worktree and child-repository observations. */
    typedef struct UmiVcsWorkingTreeEntry
    {
        UmiVcsChange change;
        char index_code;
        char worktree_code;
        int submodule;
        int child_commit_changed;
        int child_tracked_changed;
        int child_untracked;
        int unmerged;
        unsigned similarity;
        /* Rename sources and destinations are raw repository-relative bytes, not display labels. */
    } UmiVcsWorkingTreeEntry;

    /** Describe observed state only; a clean result does not prove remote publication or readiness.
     */
    typedef struct UmiVcsWorkingTreeSummary
    {
        char branch[UMI_VCS_NAME_CAPACITY];
        char upstream[UMI_VCS_NAME_CAPACITY];
        char commit_id[UMI_VCS_ID_CAPACITY];
        int branch_known;
        int commit_known;
        int unborn;
        int detached;
        int divergence_known;
        int stash_known;
        size_t ahead;
        size_t behind;
        size_t stashes;
        size_t entries;
        size_t staged;
        size_t conflicts;
        size_t untracked;
        size_t child_commits;
        size_t dirty_children;
    } UmiVcsWorkingTreeSummary;

    /** Configure a read-only observation. A zero timeout selects the bounded default of 30 seconds.
     */
    typedef struct UmiVcsWorkingTreeRequest
    {
        const char *repository_root;
        const char *git_program;
        uint32_t timeout_ms;
        const UmiCancellationToken *cancellation;
    } UmiVcsWorkingTreeRequest;

    /**
     * Parse complete NUL-delimited Git porcelain records without treating paths as lines.
     * The output pointer is assigned only on success. Input bytes remain caller-owned.
     * Unsupported or incomplete records fail the entire observation rather than hiding changes.
     */
    UmiStatus UmiVcsWorkingTreeParse(const void *bytes, size_t length,
                                     UmiVcsWorkingTree **out_tree);

    /**
     * Read status using an argument vector and explicit directory, without staging or fetching.
     * Call from a worker when used by an interactive application. Output is assigned only on
     * success.
     */
    UmiStatus UmiVcsWorkingTreeRead(const UmiVcsWorkingTreeRequest *request,
                                    UmiVcsWorkingTree **out_tree);

    /** Release an observation and all its entries. A null pointer is safe. */
    void UmiVcsWorkingTreeDestroy(UmiVcsWorkingTree *tree);

    /** Copy the summary; the caller owns the returned value. */
    UmiStatus UmiVcsWorkingTreeDescribe(const UmiVcsWorkingTree *tree,
                                        UmiVcsWorkingTreeSummary *out_summary);

    /** Borrow one entry until the observation is destroyed; an invalid index returns null. */
    const UmiVcsWorkingTreeEntry *UmiVcsWorkingTreeEntryAt(const UmiVcsWorkingTree *tree,
                                                           size_t index);

    /**
     * Replace the legacy change list and branch together after all conversions have succeeded.
     * Existing consumers keep their API while receiving paths parsed by the shared binary reader.
     */
    UmiStatus UmiVcsWorkingTreeCopyChanges(const UmiVcsWorkingTree *tree, UmiVcsChangeList *changes,
                                           UmiVcsBranch *branch);

#ifdef __cplusplus
}
#endif
#endif
