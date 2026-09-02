/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/binary_compare.h
 *
 * PURPOSE:
 *   Compare binary artifacts using size and deterministic content fingerprints.
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
#ifndef UMICOM_VCS_ADVANCED_BINARY_COMPARE_H
#define UMICOM_VCS_ADVANCED_BINARY_COMPARE_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs advanced binary compare data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedBinaryCompare {
    uint32_t struct_size; uint32_t api_version; uint64_t left_size,right_size,left_hash,right_hash;
    int same_size,same_content;
} UmiVcsAdvancedBinaryCompare;
/**
 * Provide the vcs advanced binary compare values operation used by this module and its
 * client applications.
 */
void umi_vcs_advanced_binary_compare_values(uint64_t left_size,uint64_t left_hash,uint64_t right_size,uint64_t right_hash,
                                             UmiVcsAdvancedBinaryCompare *out_result);
#ifdef __cplusplus
}
#endif
#endif
