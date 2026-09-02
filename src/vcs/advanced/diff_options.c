/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/diff_options.c
 *
 * PURPOSE:
 *   Implement deterministic user comparison options shared by every frontend.
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
#include "umicom/vcs/advanced/diff_options.h"

#include <string.h>

/*
 * Initialise vcs advanced diff options from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_diff_options_init(UmiVcsAdvancedDiffOptions *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->context_lines = 3U;
    value->detect_moves = 1;
    value->treat_crlf_as_lf = 1;
}

/*
 * Check that vcs advanced diff options satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_diff_options_validate(const UmiVcsAdvancedDiffOptions *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->whitespace > UMI_VCS_WHITESPACE_IGNORE_ALL || value->context_lines > 10000U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced diff options fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_vcs_advanced_diff_options_fingerprint(const UmiVcsAdvancedDiffOptions *value)
{
    uint64_t result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return 0U;
    result = (uint64_t)value->whitespace;
    result = result * UINT64_C(1315423911) + (uint64_t)value->context_lines;
    result = result * UINT64_C(1315423911) + (uint64_t)(value->ignore_case != 0);
    result = result * UINT64_C(1315423911) + (uint64_t)(value->detect_moves != 0);
    result = result * UINT64_C(1315423911) + (uint64_t)(value->semantic != 0);
    return result;
}
