/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/intraline_diff.h
 *
 * PURPOSE:
 *   Locate changed spans within a pair of modified lines using common prefix/suffix boundaries.
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
#ifndef UMICOM_VCS_ADVANCED_INTRALINE_DIFF_H
#define UMICOM_VCS_ADVANCED_INTRALINE_DIFF_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsAdvancedIntralineDiff {
    uint32_t struct_size; uint32_t api_version;
    size_t common_prefix,common_suffix,left_change_start,left_change_count,right_change_start,right_change_count;
    int equal;
} UmiVcsAdvancedIntralineDiff;
UmiStatus umi_vcs_advanced_intraline_diff_compute(const char *left,const char *right,UmiVcsAdvancedIntralineDiff *out_diff);
#ifdef __cplusplus
}
#endif
#endif
