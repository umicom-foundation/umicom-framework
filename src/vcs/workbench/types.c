/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/workbench/types.c
 *
 * PURPOSE:
 *   Implement shared bounded text and presentation vocabulary helpers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/workbench/types.h"

#include <string.h>

const char *umi_vcs_workbench_phase_text(UmiVcsWorkbenchPhase phase)
{
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

const char *umi_vcs_workbench_review_decision_text(
    UmiVcsWorkbenchReviewDecision decision)
{
    switch (decision) {
        case UMI_VCS_WORKBENCH_REVIEW_PENDING: return "pending";
        case UMI_VCS_WORKBENCH_REVIEW_APPROVED: return "approved";
        case UMI_VCS_WORKBENCH_REVIEW_CHANGES_REQUESTED:
            return "changes-requested";
        default: return "invalid";
    }
}

UmiStatus umi_vcs_workbench_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}
