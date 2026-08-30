/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/diff_summary.h
 *
 * PURPOSE:
 *   Aggregate normalized diff-line statistics for Source Control and compare views.
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
#ifndef UMICOM_VCS_ADVANCED_DIFF_SUMMARY_H
#define UMICOM_VCS_ADVANCED_DIFF_SUMMARY_H
#include "umicom/vcs/advanced/diff_line.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsAdvancedDiffSummary {
    uint32_t struct_size; uint32_t api_version;
    size_t context_lines,added_lines,deleted_lines,modified_lines,moved_lines,total_lines;
} UmiVcsAdvancedDiffSummary;
void umi_vcs_advanced_diff_summary_init(UmiVcsAdvancedDiffSummary *summary);
void umi_vcs_advanced_diff_summary_add(UmiVcsAdvancedDiffSummary *summary,const UmiVcsAdvancedDiffLine *line);
size_t umi_vcs_advanced_diff_summary_change_count(const UmiVcsAdvancedDiffSummary *summary);
uint32_t umi_vcs_advanced_diff_summary_change_percent(const UmiVcsAdvancedDiffSummary *summary);
#ifdef __cplusplus
}
#endif
#endif
