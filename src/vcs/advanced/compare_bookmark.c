/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/compare_bookmark.c
 *
 * PURPOSE:
 *   Persist named source locations inside long comparison sessions.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/compare_bookmark.h"

#include <string.h>

void umi_vcs_advanced_compare_bookmark_init(UmiVcsAdvancedCompareBookmark *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_compare_bookmark_validate(const UmiVcsAdvancedCompareBookmark *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->path) || value->line_number == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_compare_bookmark_set(UmiVcsAdvancedCompareBookmark *value,
                                                  const char *path,
                                                  const char *label,
                                                  size_t line_number,
                                                  size_t change_index)
{
    UmiStatus status;
    if (value == NULL || line_number == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->path, sizeof(value->path), path);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->label, sizeof(value->label), label);
    if (status != UMI_STATUS_OK) return status;
    value->line_number = line_number;
    value->change_index = change_index;
    value->fingerprint = umi_vcs_advanced_hash_text(path) ^ (uint64_t)line_number;
    return umi_vcs_advanced_compare_bookmark_validate(value);
}
