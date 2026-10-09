/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/working_tree_legacy.c
 * PURPOSE: Publish detailed Git observations through the existing provider-neutral change API.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/working_tree.h"
#include <limits.h>
#include <string.h>

/* Preserve the old list until every exact path and branch counter fits its existing API. */
UmiStatus UmiVcsWorkingTreeCopyChanges(const UmiVcsWorkingTree *tree, UmiVcsChangeList *changes,
                                       UmiVcsBranch *branch)
{
    UmiVcsWorkingTreeSummary summary;
    UmiVcsBranch next_branch = {0};
    UmiVcsChangeList *next_changes = NULL;
    UmiStatus status;
    if (tree == NULL || changes == NULL || branch == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiVcsWorkingTreeDescribe(tree, &summary);
    if (status != UMI_STATUS_OK)
        return status;
    if (summary.ahead > (size_t)INT_MAX || summary.behind > (size_t)INT_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(next_branch.name, summary.branch, sizeof(next_branch.name));
    memcpy(next_branch.upstream, summary.upstream, sizeof(next_branch.upstream));
    next_branch.current = summary.branch_known;
    next_branch.detached = summary.detached;
    next_branch.ahead = (int)summary.ahead;
    next_branch.behind = (int)summary.behind;
    status = umi_vcs_change_list_create(&next_changes);
    for (size_t index = 0U; status == UMI_STATUS_OK && index < summary.entries; ++index)
        status =
            umi_vcs_change_list_add(next_changes, &UmiVcsWorkingTreeEntryAt(tree, index)->change);
    if (status == UMI_STATUS_OK)
    {
        status = UmiVcsChangeListExchange(changes, next_changes);
        if (status == UMI_STATUS_OK)
            *branch = next_branch;
    }
    umi_vcs_change_list_destroy(next_changes);
    return status;
}
