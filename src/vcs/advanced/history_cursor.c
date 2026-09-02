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

/*
 * Initialise vcs advanced history cursor from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_history_cursor_init(UmiVcsAdvancedHistoryCursor *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->limit = 100U;
}

/*
 * Check that vcs advanced history cursor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_history_cursor_validate(const UmiVcsAdvancedHistoryCursor *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->limit == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced history cursor advance operation used by this module and its
 * client applications.
 */
void umi_vcs_advanced_history_cursor_advance(UmiVcsAdvancedHistoryCursor *value,
                                                size_t returned,
                                                int has_more)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    value->returned = returned;
    value->offset += returned;
    value->has_more = has_more != 0;
}
