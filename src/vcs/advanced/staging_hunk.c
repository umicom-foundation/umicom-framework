/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/staging_hunk.c
 *
 * PURPOSE:
 *   Implement a selectable diff hunk for partial staging and unstaging.
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
#include "umicom/vcs/advanced/staging_hunk.h"

#include <string.h>

void umi_vcs_advanced_staging_hunk_init(UmiVcsAdvancedStagingHunk *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_staging_hunk_validate(const UmiVcsAdvancedStagingHunk *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->path) || (value->old_count == 0U && value->new_count == 0U))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_staging_hunk_set(UmiVcsAdvancedStagingHunk *value,
                                              const char *path,
                                              size_t old_start,
                                              size_t old_count,
                                              size_t new_start,
                                              size_t new_count,
                                              const char *patch_text)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->path, sizeof(value->path), path);
    if (status != UMI_STATUS_OK) return status;
    value->old_start = old_start;
    value->old_count = old_count;
    value->new_start = new_start;
    value->new_count = new_count;
    value->fingerprint = umi_vcs_advanced_hash_text(patch_text);
    value->selected = 1;
    return umi_vcs_advanced_staging_hunk_validate(value);
}
