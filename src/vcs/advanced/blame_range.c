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

/*
 * Initialise vcs advanced blame range from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_blame_range_init(UmiVcsAdvancedBlameRange *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

/*
 * Check that vcs advanced blame range satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_blame_range_validate(const UmiVcsAdvancedBlameRange *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->first_line == 0U || value->last_line < value->first_line)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Copy vcs advanced blame range into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_blame_range_set(UmiVcsAdvancedBlameRange *value,
                                             size_t first_line,
                                             size_t last_line)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || first_line == 0U || last_line < first_line) return UMI_STATUS_INVALID_ARGUMENT;
    value->first_line = first_line;
    value->last_line = last_line;
    return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by vcs advanced blame range without changing
 * their state.
 */
size_t umi_vcs_advanced_blame_range_count(const UmiVcsAdvancedBlameRange *value)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_blame_range_validate(value) != UMI_STATUS_OK) return 0U;
    return value->last_line - value->first_line + 1U;
}
