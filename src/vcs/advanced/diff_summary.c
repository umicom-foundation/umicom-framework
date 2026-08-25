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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/diff_summary.h"

#include <string.h>

void umi_vcs_advanced_diff_summary_init(UmiVcsAdvancedDiffSummary *summary)
{
    if (summary == NULL) {
        return;
    }
    (void)memset(summary, 0, sizeof(*summary));
    summary->struct_size = (uint32_t)sizeof(*summary);
    summary->api_version = UMI_VCS_ADVANCED_API_VERSION;
}

void umi_vcs_advanced_diff_summary_add(
    UmiVcsAdvancedDiffSummary *summary,
    const UmiVcsAdvancedDiffLine *line)
{
    if (summary == NULL ||
        umi_vcs_advanced_diff_line_validate(line) != UMI_STATUS_OK) {
        return;
    }

    summary->total_lines += 1U;
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

size_t umi_vcs_advanced_diff_summary_change_count(
    const UmiVcsAdvancedDiffSummary *summary)
{
    if (summary == NULL) {
        return 0U;
    }
    return summary->added_lines +
           summary->deleted_lines +
           summary->modified_lines +
           summary->moved_lines;
}

uint32_t umi_vcs_advanced_diff_summary_change_percent(
    const UmiVcsAdvancedDiffSummary *summary)
{
    size_t changes;
    if (summary == NULL || summary->total_lines == 0U) {
        return 0U;
    }
    changes = umi_vcs_advanced_diff_summary_change_count(summary);
    return (uint32_t)((changes * 100U) / summary->total_lines);
}
