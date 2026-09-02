/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/merge_base.c
 *
 * PURPOSE:
 *   Implement deterministic merge-base candidate selection.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/merge_base.h"

#include <string.h>

/*
 * Initialise vcs advanced merge base from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_merge_base_init(UmiVcsAdvancedMergeBase *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) {
        return;
    }
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
}

/*
 * Add vcs advanced merge base only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_vcs_advanced_merge_base_add(UmiVcsAdvancedMergeBase *value,
                                           const char *oid,
                                           uint32_t generation,
                                           uint64_t timestamp)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || !umi_vcs_advanced_text_present(oid)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value->candidate_count >= 16U) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_vcs_advanced_copy_text(
        value->candidates[value->candidate_count].oid,
        sizeof(value->candidates[0].oid),
        oid);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    value->candidates[value->candidate_count].generation = generation;
    value->candidates[value->candidate_count].timestamp_seconds = timestamp;
    value->candidate_count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced merge base select operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_advanced_merge_base_select(UmiVcsAdvancedMergeBase *value)
{
    size_t i;
    size_t best;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->candidate_count == 0U) {
        return UMI_STATUS_NOT_FOUND;
    }

    best = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 1U; i < value->candidate_count; ++i) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (value->candidates[i].generation >
                value->candidates[best].generation ||
            (value->candidates[i].generation ==
                 value->candidates[best].generation &&
             value->candidates[i].timestamp_seconds >
                 value->candidates[best].timestamp_seconds)) {
            best = i;
        }
    }

    return umi_vcs_advanced_copy_text(
        value->selected_oid,
        sizeof(value->selected_oid),
        value->candidates[best].oid);
}
