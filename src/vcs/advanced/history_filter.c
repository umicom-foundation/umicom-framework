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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/history_filter.h"
#include <ctype.h>
#include <string.h>
static int contains_text(const char *haystack, const char *needle, int case_sensitive)
{
    size_t i, j, hlen, nlen;
    if (!umi_vcs_advanced_text_present(needle)) return 1;
    if (haystack == NULL) return 0;
    hlen = strlen(haystack); nlen = strlen(needle);
    if (nlen > hlen) return 0;
    for (i = 0U; i + nlen <= hlen; ++i) {
        for (j = 0U; j < nlen; ++j) {
            unsigned char a = (unsigned char)haystack[i+j];
            unsigned char b = (unsigned char)needle[j];
            if (!case_sensitive) { a = (unsigned char)tolower(a); b = (unsigned char)tolower(b); }
            if (a != b) break;
        }
        if (j == nlen) return 1;
    }
    return 0;
}
void umi_vcs_advanced_history_filter_init(UmiVcsAdvancedHistoryFilter *filter)
{
    if (filter == NULL) return;
    (void)memset(filter, 0, sizeof(*filter));
    filter->struct_size = (uint32_t)sizeof(*filter);
    filter->api_version = UMI_VCS_ADVANCED_API_VERSION;
}
UmiStatus umi_vcs_advanced_history_filter_validate(const UmiVcsAdvancedHistoryFilter *filter)
{
    if (filter == NULL || filter->struct_size < sizeof(*filter) ||
        filter->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (filter->merges_only && filter->exclude_merges) ||
        (filter->until_seconds != 0U && filter->since_seconds > filter->until_seconds))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
int umi_vcs_advanced_history_filter_match(const UmiVcsAdvancedHistoryFilter *filter,
                                           const UmiVcsAdvancedCommitNode *node)
{
    if (umi_vcs_advanced_history_filter_validate(filter) != UMI_STATUS_OK ||
        umi_vcs_advanced_commit_node_validate(node) != UMI_STATUS_OK) return 0;
    if (filter->since_seconds != 0U && node->timestamp_seconds < filter->since_seconds) return 0;
    if (filter->until_seconds != 0U && node->timestamp_seconds > filter->until_seconds) return 0;
    if (filter->merges_only && !node->merge_commit) return 0;
    if (filter->exclude_merges && node->merge_commit) return 0;
    if (!contains_text(node->author, filter->author_contains, filter->case_sensitive)) return 0;
    if (!contains_text(node->subject, filter->subject_contains, filter->case_sensitive)) return 0;
    return 1;
}
