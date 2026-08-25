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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/merge_base.h"

#include <string.h>

void umi_vcs_advanced_merge_base_init(UmiVcsAdvancedMergeBase *value)
{
    if (value == NULL) {
        return;
    }
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
}

UmiStatus umi_vcs_advanced_merge_base_add(UmiVcsAdvancedMergeBase *value,
                                           const char *oid,
                                           uint32_t generation,
                                           uint64_t timestamp)
{
    UmiStatus status;
    if (value == NULL || !umi_vcs_advanced_text_present(oid)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (value->candidate_count >= 16U) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_vcs_advanced_copy_text(
        value->candidates[value->candidate_count].oid,
        sizeof(value->candidates[0].oid),
        oid);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    value->candidates[value->candidate_count].generation = generation;
    value->candidates[value->candidate_count].timestamp_seconds = timestamp;
    value->candidate_count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_merge_base_select(UmiVcsAdvancedMergeBase *value)
{
    size_t i;
    size_t best;

    if (value == NULL || value->candidate_count == 0U) {
        return UMI_STATUS_NOT_FOUND;
    }

    best = 0U;
    for (i = 1U; i < value->candidate_count; ++i) {
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
