/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/binary_compare.c
 *
 * PURPOSE:
 *   Implement binary artifact equality from size and fingerprint evidence.
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

#include "umicom/vcs/advanced/binary_compare.h"

#include <string.h>

void umi_vcs_advanced_binary_compare_values(
    uint64_t left_size,
    uint64_t left_hash,
    uint64_t right_size,
    uint64_t right_hash,
    UmiVcsAdvancedBinaryCompare *out_result)
{
    if (out_result == NULL) {
        return;
    }
    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->struct_size = (uint32_t)sizeof(*out_result);
    out_result->api_version = UMI_VCS_ADVANCED_API_VERSION;
    out_result->left_size = left_size;
    out_result->right_size = right_size;
    out_result->left_hash = left_hash;
    out_result->right_hash = right_hash;
    out_result->same_size = left_size == right_size;
    out_result->same_content =
        out_result->same_size && left_hash == right_hash;
}
