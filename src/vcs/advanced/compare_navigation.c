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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/compare_navigation.h"

#include <string.h>

/*
 * Initialise vcs advanced compare navigation from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_advanced_compare_navigation_init(UmiVcsAdvancedCompareNavigation *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->wrap = 1;
}

/*
 * Check that vcs advanced compare navigation satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_compare_navigation_validate(const UmiVcsAdvancedCompareNavigation *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->change_count > 0U && value->current_index >= value->change_count)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced compare navigation next operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_compare_navigation_next(UmiVcsAdvancedCompareNavigation *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->change_count == 0U) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->current_index + 1U < value->change_count) {
        value->current_index += 1U;
        return 1;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->wrap) {
        value->current_index = 0U;
        return 1;
    }
    return 0;
}
/*
 * Provide the vcs advanced compare navigation previous operation used by this module and
 * its client applications.
 */
int umi_vcs_advanced_compare_navigation_previous(UmiVcsAdvancedCompareNavigation *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->change_count == 0U) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->current_index > 0U) {
        value->current_index -= 1U;
        return 1;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->wrap) {
        value->current_index = value->change_count - 1U;
        return 1;
    }
    return 0;
}
