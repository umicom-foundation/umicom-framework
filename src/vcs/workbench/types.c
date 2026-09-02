/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/workbench/types.c
 *
 * PURPOSE:
 *   Implement shared bounded text and presentation vocabulary helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/workbench/types.h"

#include <string.h>

/*
 * Provide the vcs workbench phase text operation used by this module and its client
 * applications.
 */
const char *umi_vcs_workbench_phase_text(UmiVcsWorkbenchPhase phase)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (phase) {
        case UMI_VCS_WORKBENCH_IDLE: return "idle";
        case UMI_VCS_WORKBENCH_COMPARING: return "comparing";
        case UMI_VCS_WORKBENCH_DIRECTORY_COMPARING:
            return "directory-comparing";
        case UMI_VCS_WORKBENCH_MERGING: return "merging";
        case UMI_VCS_WORKBENCH_PARTIAL_STAGING: return "partial-staging";
        case UMI_VCS_WORKBENCH_REVIEWING: return "reviewing";
        case UMI_VCS_WORKBENCH_FAILED: return "failed";
        default: return "invalid";
    }
}

/*
 * Provide the vcs workbench review decision text operation used by this module and its
 * client applications.
 */
const char *umi_vcs_workbench_review_decision_text(
    UmiVcsWorkbenchReviewDecision decision)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (decision) {
        case UMI_VCS_WORKBENCH_REVIEW_PENDING: return "pending";
        case UMI_VCS_WORKBENCH_REVIEW_APPROVED: return "approved";
        case UMI_VCS_WORKBENCH_REVIEW_CHANGES_REQUESTED:
            return "changes-requested";
        default: return "invalid";
    }
}

/*
 * Provide the vcs workbench copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workbench_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}
