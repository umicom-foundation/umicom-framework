/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/history_cursor.c
 *
 * PURPOSE:
 *   Track deterministic pagination through large repository histories.
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
#include "umicom/vcs/advanced/history_cursor.h"

#include <string.h>

void umi_vcs_advanced_history_cursor_init(UmiVcsAdvancedHistoryCursor *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->limit = 100U;
}

UmiStatus umi_vcs_advanced_history_cursor_validate(const UmiVcsAdvancedHistoryCursor *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->limit == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

void umi_vcs_advanced_history_cursor_advance(UmiVcsAdvancedHistoryCursor *value,
                                                size_t returned,
                                                int has_more)
{
    if (value == NULL) return;
    value->returned = returned;
    value->offset += returned;
    value->has_more = has_more != 0;
}
