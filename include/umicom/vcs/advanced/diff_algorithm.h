/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/diff_algorithm.h
 *
 * PURPOSE:
 *   Choose comparison algorithms deterministically from content size, mode and binary state.
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
#ifndef UMICOM_VCS_ADVANCED_DIFF_ALGORITHM_H
#define UMICOM_VCS_ADVANCED_DIFF_ALGORITHM_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named vcs advanced diff algorithm values accepted by this public contract.
 */
typedef enum UmiVcsAdvancedDiffAlgorithm {
    UMI_VCS_DIFF_ALGORITHM_MYERS=0, UMI_VCS_DIFF_ALGORITHM_PATIENCE=1,
    UMI_VCS_DIFF_ALGORITHM_HISTOGRAM=2, UMI_VCS_DIFF_ALGORITHM_SEMANTIC=3,
    UMI_VCS_DIFF_ALGORITHM_BINARY=4
} UmiVcsAdvancedDiffAlgorithm;
/**
 * Provide the vcs advanced diff algorithm select operation used by this module and its
 * client applications.
 */
UmiVcsAdvancedDiffAlgorithm umi_vcs_advanced_diff_algorithm_select(size_t left_lines,size_t right_lines,
                                                                    int binary,int semantic,int many_duplicates);
/**
 * Provide the vcs advanced diff algorithm text operation used by this module and its
 * client applications.
 */
const char *umi_vcs_advanced_diff_algorithm_text(UmiVcsAdvancedDiffAlgorithm algorithm);
#ifdef __cplusplus
}
#endif
#endif
