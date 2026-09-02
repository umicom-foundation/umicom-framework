/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/intraline_diff.c
 *
 * PURPOSE:
 *   Implement changed-span location inside pairs of modified lines.
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

#include "umicom/vcs/advanced/intraline_diff.h"

#include <string.h>

/*
 * Provide the vcs advanced intraline diff compute operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_intraline_diff_compute(
    const char *left,
    const char *right,
    UmiVcsAdvancedIntralineDiff *out_diff)
{
    size_t left_length;
    size_t right_length;
    size_t prefix = 0U;
    size_t suffix = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL || out_diff == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_diff, 0, sizeof(*out_diff));
    out_diff->struct_size = (uint32_t)sizeof(*out_diff);
    out_diff->api_version = UMI_VCS_ADVANCED_API_VERSION;

    left_length = strlen(left);
    right_length = strlen(right);

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (prefix < left_length &&
           prefix < right_length &&
           left[prefix] == right[prefix]) {
        prefix += 1U;
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (suffix < left_length - prefix &&
           suffix < right_length - prefix &&
           left[left_length - 1U - suffix] ==
               right[right_length - 1U - suffix]) {
        suffix += 1U;
    }

    out_diff->common_prefix = prefix;
    out_diff->common_suffix = suffix;
    out_diff->left_change_start = prefix;
    out_diff->right_change_start = prefix;
    out_diff->left_change_count = left_length - prefix - suffix;
    out_diff->right_change_count = right_length - prefix - suffix;
    out_diff->equal = left_length == right_length && prefix == left_length;
    return UMI_STATUS_OK;
}
