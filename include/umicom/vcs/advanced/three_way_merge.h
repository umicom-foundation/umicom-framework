/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/three_way_merge.h
 *
 * PURPOSE:
 *   Perform conservative whole-text three-way merges with explicit conflict markers.
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
#ifndef UMICOM_VCS_ADVANCED_THREE_WAY_MERGE_H
#define UMICOM_VCS_ADVANCED_THREE_WAY_MERGE_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs advanced three way merge result data shared with callers of this
 * public contract.
 */
typedef struct UmiVcsAdvancedThreeWayMergeResult {
    uint32_t struct_size; uint32_t api_version;
    char text[UMI_VCS_ADVANCED_MERGE_TEXT_CAPACITY];
    size_t conflict_count; int clean; uint64_t fingerprint;
} UmiVcsAdvancedThreeWayMergeResult;
/**
 * Provide the vcs advanced three way merge text operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_three_way_merge_text(const char *base,const char *ours,const char *theirs,
                                                  UmiVcsAdvancedThreeWayMergeResult *out_result);
#ifdef __cplusplus
}
#endif
#endif
