/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/diff_summary.c
 *
 * PURPOSE:
 *   Implement normalized diff-line aggregation and change percentages.
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

#include "umicom/vcs/advanced/diff_summary.h"

#include <string.h>

/*
 * Initialise vcs advanced diff summary from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_diff_summary_init(UmiVcsAdvancedDiffSummary *summary)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (summary == NULL) {
        return;
    }
    (void)memset(summary, 0, sizeof(*summary));
    summary->struct_size = (uint32_t)sizeof(*summary);
    summary->api_version = UMI_VCS_ADVANCED_API_VERSION;
}

/*
 * Add vcs advanced diff summary only after its inputs and available capacity have been
 * checked.
 */
void umi_vcs_advanced_diff_summary_add(
    UmiVcsAdvancedDiffSummary *summary,
    const UmiVcsAdvancedDiffLine *line)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (summary == NULL ||
        umi_vcs_advanced_diff_line_validate(line) != UMI_STATUS_OK) {
        return;
    }

    summary->total_lines += 1U;
    /* Select the behaviour associated with the requested command or state value. */
    switch (line->kind) {
        case UMI_VCS_DIFF_CONTEXT:
            summary->context_lines += 1U;
            break;
        case UMI_VCS_DIFF_ADDED:
            summary->added_lines += 1U;
            break;
        case UMI_VCS_DIFF_DELETED:
            summary->deleted_lines += 1U;
            break;
        case UMI_VCS_DIFF_MODIFIED:
            summary->modified_lines += 1U;
            break;
        case UMI_VCS_DIFF_MOVED:
            summary->moved_lines += 1U;
            break;
        default:
            break;
    }
}

/*
 * Return the number of records represented by vcs advanced diff summary change without
 * changing their state.
 */
size_t umi_vcs_advanced_diff_summary_change_count(
    const UmiVcsAdvancedDiffSummary *summary)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (summary == NULL) {
        return 0U;
    }
    return summary->added_lines +
           summary->deleted_lines +
           summary->modified_lines +
           summary->moved_lines;
}

/*
 * Provide the vcs advanced diff summary change percent operation used by this module and
 * its client applications.
 */
uint32_t umi_vcs_advanced_diff_summary_change_percent(
    const UmiVcsAdvancedDiffSummary *summary)
{
    size_t changes;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (summary == NULL || summary->total_lines == 0U) {
        return 0U;
    }
    changes = umi_vcs_advanced_diff_summary_change_count(summary);
    return (uint32_t)((changes * 100U) / summary->total_lines);
}
