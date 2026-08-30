/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/blame_range.c
 *
 * PURPOSE:
 *   Represent and normalize line ranges requested from blame/history providers.
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
#include "umicom/vcs/advanced/blame_range.h"

#include <string.h>

void umi_vcs_advanced_blame_range_init(UmiVcsAdvancedBlameRange *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_blame_range_validate(const UmiVcsAdvancedBlameRange *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->first_line == 0U || value->last_line < value->first_line)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_blame_range_set(UmiVcsAdvancedBlameRange *value,
                                             size_t first_line,
                                             size_t last_line)
{
    if (value == NULL || first_line == 0U || last_line < first_line) return UMI_STATUS_INVALID_ARGUMENT;
    value->first_line = first_line;
    value->last_line = last_line;
    return UMI_STATUS_OK;
}
size_t umi_vcs_advanced_blame_range_count(const UmiVcsAdvancedBlameRange *value)
{
    if (umi_vcs_advanced_blame_range_validate(value) != UMI_STATUS_OK) return 0U;
    return value->last_line - value->first_line + 1U;
}
