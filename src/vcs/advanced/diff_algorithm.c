/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/diff_algorithm.c
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
#include "umicom/vcs/advanced/diff_algorithm.h"
/*
 * Provide the vcs advanced diff algorithm select operation used by this module and its
 * client applications.
 */
UmiVcsAdvancedDiffAlgorithm umi_vcs_advanced_diff_algorithm_select(size_t left_lines,size_t right_lines,
                                                                    int binary,int semantic,int many_duplicates)
{
    size_t total=left_lines+right_lines;
    /* Apply this branch only when its contract condition is satisfied. */
    if(binary)return UMI_VCS_DIFF_ALGORITHM_BINARY;
    /* Apply this branch only when its contract condition is satisfied. */
    if(semantic)return UMI_VCS_DIFF_ALGORITHM_SEMANTIC;
    /* Apply this branch only when its contract condition is satisfied. */
    if(total>50000U)return UMI_VCS_DIFF_ALGORITHM_HISTOGRAM;
    /* Apply this branch only when its contract condition is satisfied. */
    if(many_duplicates)return UMI_VCS_DIFF_ALGORITHM_PATIENCE;
    return UMI_VCS_DIFF_ALGORITHM_MYERS;
}
/*
 * Provide the vcs advanced diff algorithm text operation used by this module and its
 * client applications.
 */
const char *umi_vcs_advanced_diff_algorithm_text(UmiVcsAdvancedDiffAlgorithm algorithm)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch(algorithm){case UMI_VCS_DIFF_ALGORITHM_MYERS:return "myers";case UMI_VCS_DIFF_ALGORITHM_PATIENCE:return "patience";
    case UMI_VCS_DIFF_ALGORITHM_HISTOGRAM:return "histogram";case UMI_VCS_DIFF_ALGORITHM_SEMANTIC:return "semantic";
    case UMI_VCS_DIFF_ALGORITHM_BINARY:return "binary";default:return "invalid";}
}
