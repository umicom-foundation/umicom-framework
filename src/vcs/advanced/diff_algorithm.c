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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/diff_algorithm.h"
UmiVcsAdvancedDiffAlgorithm umi_vcs_advanced_diff_algorithm_select(size_t left_lines,size_t right_lines,
                                                                    int binary,int semantic,int many_duplicates)
{
    size_t total=left_lines+right_lines;
    if(binary)return UMI_VCS_DIFF_ALGORITHM_BINARY;
    if(semantic)return UMI_VCS_DIFF_ALGORITHM_SEMANTIC;
    if(total>50000U)return UMI_VCS_DIFF_ALGORITHM_HISTOGRAM;
    if(many_duplicates)return UMI_VCS_DIFF_ALGORITHM_PATIENCE;
    return UMI_VCS_DIFF_ALGORITHM_MYERS;
}
const char *umi_vcs_advanced_diff_algorithm_text(UmiVcsAdvancedDiffAlgorithm algorithm)
{
    switch(algorithm){case UMI_VCS_DIFF_ALGORITHM_MYERS:return "myers";case UMI_VCS_DIFF_ALGORITHM_PATIENCE:return "patience";
    case UMI_VCS_DIFF_ALGORITHM_HISTOGRAM:return "histogram";case UMI_VCS_DIFF_ALGORITHM_SEMANTIC:return "semantic";
    case UMI_VCS_DIFF_ALGORITHM_BINARY:return "binary";default:return "invalid";}
}
