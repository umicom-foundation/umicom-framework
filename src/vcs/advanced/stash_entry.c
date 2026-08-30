/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/stash_entry.c
 *
 * PURPOSE:
 *   Implement one stash entry with base/index/worktree identity and metadata.
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
#include "umicom/vcs/advanced/stash_entry.h"

#include <string.h>

void umi_vcs_advanced_stash_entry_init(UmiVcsAdvancedStashEntry *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_stash_entry_validate(const UmiVcsAdvancedStashEntry *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->stash_ref))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_stash_entry_set(UmiVcsAdvancedStashEntry *value,
                                             size_t index,
                                             const char *stash_ref,
                                             const char *base_oid,
                                             const char *message,
                                             const char *branch)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->index = index;
    status = umi_vcs_advanced_copy_text(value->stash_ref, sizeof(value->stash_ref), stash_ref);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->base_oid, sizeof(value->base_oid), base_oid);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->message, sizeof(value->message), message);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->branch, sizeof(value->branch), branch);
    if (status != UMI_STATUS_OK) return status;
    return umi_vcs_advanced_stash_entry_validate(value);
}
