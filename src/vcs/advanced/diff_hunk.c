/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/diff_hunk.c
 *
 * PURPOSE:
 *   Implement normalized change blocks for navigation and partial operations.
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
#include "umicom/vcs/advanced/diff_hunk.h"

#include <string.h>

void umi_vcs_advanced_diff_hunk_init(UmiVcsAdvancedDiffHunk *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_diff_hunk_validate(const UmiVcsAdvancedDiffHunk *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->old_count == 0U && value->new_count == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

void umi_vcs_advanced_diff_hunk_set_counts(UmiVcsAdvancedDiffHunk *value,
                                             size_t added,
                                             size_t deleted,
                                             size_t modified)
{
    if (value == NULL) return;
    value->added_lines = added;
    value->deleted_lines = deleted;
    value->modified_lines = modified;
}
size_t umi_vcs_advanced_diff_hunk_change_count(const UmiVcsAdvancedDiffHunk *value)
{
    if (value == NULL) return 0U;
    return value->added_lines + value->deleted_lines + value->modified_lines;
}
