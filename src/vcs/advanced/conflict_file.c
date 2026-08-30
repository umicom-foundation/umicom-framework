/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/conflict_file.c
 *
 * PURPOSE:
 *   Capture index-stage availability and resolution state for a conflicted path.
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
#include "umicom/vcs/advanced/conflict_file.h"

#include <string.h>

void umi_vcs_advanced_conflict_file_init(UmiVcsAdvancedConflictFile *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->choice = UMI_VCS_CONFLICT_UNRESOLVED;
    value->state = UMI_VCS_ADVANCED_STATE_CONFLICTED;
}

UmiStatus umi_vcs_advanced_conflict_file_validate(const UmiVcsAdvancedConflictFile *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->path) || (!value->has_ours && !value->has_theirs))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_conflict_file_set(UmiVcsAdvancedConflictFile *value,
                                               const char *path,
                                               int has_base,
                                               int has_ours,
                                               int has_theirs)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->path, sizeof(value->path), path);
    if (status != UMI_STATUS_OK) return status;
    value->has_base = has_base != 0;
    value->has_ours = has_ours != 0;
    value->has_theirs = has_theirs != 0;
    return umi_vcs_advanced_conflict_file_validate(value);
}
