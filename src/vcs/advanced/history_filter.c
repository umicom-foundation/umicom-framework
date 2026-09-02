/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/history_filter.c
 *
 * PURPOSE:
 *   Filter commit-history nodes by author, subject, path/time hints and merge policy.
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
#include "umicom/vcs/advanced/history_filter.h"
#include <ctype.h>
#include <string.h>
/* Provide the contains text operation used by this module and its client applications. */
static int contains_text(const char *haystack, const char *needle, int case_sensitive)
{
    size_t i, j, hlen, nlen;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_vcs_advanced_text_present(needle)) return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (haystack == NULL) return 0;
    hlen = strlen(haystack); nlen = strlen(needle);
    /* Apply this branch only when its contract condition is satisfied. */
    if (nlen > hlen) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i + nlen <= hlen; ++i) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (j = 0U; j < nlen; ++j) {
            unsigned char a = (unsigned char)haystack[i+j];
            unsigned char b = (unsigned char)needle[j];
            /* Apply this branch only when its contract condition is satisfied. */
            if (!case_sensitive) { a = (unsigned char)tolower(a); b = (unsigned char)tolower(b); }
            /* Apply this branch only when its contract condition is satisfied. */
            if (a != b) break;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (j == nlen) return 1;
    }
    return 0;
}
/*
 * Initialise vcs advanced history filter from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_history_filter_init(UmiVcsAdvancedHistoryFilter *filter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter == NULL) return;
    (void)memset(filter, 0, sizeof(*filter));
    filter->struct_size = (uint32_t)sizeof(*filter);
    filter->api_version = UMI_VCS_ADVANCED_API_VERSION;
}
/*
 * Check that vcs advanced history filter satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_history_filter_validate(const UmiVcsAdvancedHistoryFilter *filter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter == NULL || filter->struct_size < sizeof(*filter) ||
        filter->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (filter->merges_only && filter->exclude_merges) ||
        (filter->until_seconds != 0U && filter->since_seconds > filter->until_seconds))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Provide the vcs advanced history filter match operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_history_filter_match(const UmiVcsAdvancedHistoryFilter *filter,
                                           const UmiVcsAdvancedCommitNode *node)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_history_filter_validate(filter) != UMI_STATUS_OK ||
        umi_vcs_advanced_commit_node_validate(node) != UMI_STATUS_OK) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (filter->since_seconds != 0U && node->timestamp_seconds < filter->since_seconds) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (filter->until_seconds != 0U && node->timestamp_seconds > filter->until_seconds) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (filter->merges_only && !node->merge_commit) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (filter->exclude_merges && node->merge_commit) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!contains_text(node->author, filter->author_contains, filter->case_sensitive)) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!contains_text(node->subject, filter->subject_contains, filter->case_sensitive)) return 0;
    return 1;
}
