/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/semantic_diff.h
 *
 * PURPOSE:
 *   Estimate structural similarity after whitespace and identifier-boundary normalization.
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
#ifndef UMICOM_VCS_ADVANCED_SEMANTIC_DIFF_H
#define UMICOM_VCS_ADVANCED_SEMANTIC_DIFF_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs advanced semantic diff data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedSemanticDiff {
    uint32_t struct_size; uint32_t api_version;
    uint64_t left_fingerprint,right_fingerprint;
    size_t left_token_count,right_token_count,common_token_count;
    uint32_t similarity_percent; int equivalent;
} UmiVcsAdvancedSemanticDiff;
/**
 * Provide the vcs advanced semantic diff compare operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_semantic_diff_compare(const char *left,const char *right,UmiVcsAdvancedSemanticDiff *out_result);
#ifdef __cplusplus
}
#endif
#endif
