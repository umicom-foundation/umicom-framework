/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/conflict_hunk.c
 *
 * PURPOSE:
 *   Implement aligned base/ours/theirs ranges for one textual conflict.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/conflict_hunk.h"

#include <string.h>

void umi_vcs_advanced_conflict_hunk_init(UmiVcsAdvancedConflictHunk *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_conflict_hunk_validate(const UmiVcsAdvancedConflictHunk *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->ours_count == 0U && value->theirs_count == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_conflict_hunk_set(UmiVcsAdvancedConflictHunk *value,
                                               size_t base_start,
                                               size_t base_count,
                                               size_t ours_start,
                                               size_t ours_count,
                                               size_t theirs_start,
                                               size_t theirs_count,
                                               const char *identity_text)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->base_start = base_start; value->base_count = base_count;
    value->ours_start = ours_start; value->ours_count = ours_count;
    value->theirs_start = theirs_start; value->theirs_count = theirs_count;
    value->fingerprint = umi_vcs_advanced_hash_text(identity_text);
    value->choice = UMI_VCS_CONFLICT_UNRESOLVED;
    return umi_vcs_advanced_conflict_hunk_validate(value);
}
