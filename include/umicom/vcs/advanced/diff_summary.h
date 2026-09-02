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
/**
 * Represent the vcs advanced diff summary data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedDiffSummary {
    uint32_t struct_size; uint32_t api_version;
    size_t context_lines,added_lines,deleted_lines,modified_lines,moved_lines,total_lines;
} UmiVcsAdvancedDiffSummary;
/**
 * Initialise vcs advanced diff summary from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_diff_summary_init(UmiVcsAdvancedDiffSummary *summary);
/**
 * Add vcs advanced diff summary only after its inputs and available capacity have been
 * checked.
 */
void umi_vcs_advanced_diff_summary_add(UmiVcsAdvancedDiffSummary *summary,const UmiVcsAdvancedDiffLine *line);
/**
 * Return the number of records represented by vcs advanced diff summary change without
 * changing their state.
 */
size_t umi_vcs_advanced_diff_summary_change_count(const UmiVcsAdvancedDiffSummary *summary);
/**
 * Provide the vcs advanced diff summary change percent operation used by this module and
 * its client applications.
 */
uint32_t umi_vcs_advanced_diff_summary_change_percent(const UmiVcsAdvancedDiffSummary *summary);
#ifdef __cplusplus
}
#endif
#endif
