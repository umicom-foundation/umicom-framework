/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/compare_navigation.c
 *
 * PURPOSE:
 *   Track deterministic next/previous change navigation in comparison sessions.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/compare_navigation.h"

#include <string.h>

void umi_vcs_advanced_compare_navigation_init(UmiVcsAdvancedCompareNavigation *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->wrap = 1;
}

UmiStatus umi_vcs_advanced_compare_navigation_validate(const UmiVcsAdvancedCompareNavigation *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->change_count > 0U && value->current_index >= value->change_count)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

int umi_vcs_advanced_compare_navigation_next(UmiVcsAdvancedCompareNavigation *value)
{
    if (value == NULL || value->change_count == 0U) return 0;
    if (value->current_index + 1U < value->change_count) {
        value->current_index += 1U;
        return 1;
    }
    if (value->wrap) {
        value->current_index = 0U;
        return 1;
    }
    return 0;
}
int umi_vcs_advanced_compare_navigation_previous(UmiVcsAdvancedCompareNavigation *value)
{
    if (value == NULL || value->change_count == 0U) return 0;
    if (value->current_index > 0U) {
        value->current_index -= 1U;
        return 1;
    }
    if (value->wrap) {
        value->current_index = value->change_count - 1U;
        return 1;
    }
    return 0;
}
