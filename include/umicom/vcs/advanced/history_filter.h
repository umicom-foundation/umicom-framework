/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/history_filter.h
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
#ifndef UMICOM_VCS_ADVANCED_HISTORY_FILTER_H
#define UMICOM_VCS_ADVANCED_HISTORY_FILTER_H
#include "umicom/vcs/advanced/commit_node.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsAdvancedHistoryFilter {
    uint32_t struct_size;
    uint32_t api_version;
    char author_contains[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char subject_contains[UMI_VCS_ADVANCED_TEXT_CAPACITY];
    char path_hint[UMI_VCS_ADVANCED_PATH_CAPACITY];
    uint64_t since_seconds;
    uint64_t until_seconds;
    int merges_only;
    int exclude_merges;
    int case_sensitive;
} UmiVcsAdvancedHistoryFilter;
void umi_vcs_advanced_history_filter_init(UmiVcsAdvancedHistoryFilter *filter);
UmiStatus umi_vcs_advanced_history_filter_validate(const UmiVcsAdvancedHistoryFilter *filter);
int umi_vcs_advanced_history_filter_match(const UmiVcsAdvancedHistoryFilter *filter,
                                           const UmiVcsAdvancedCommitNode *node);
#ifdef __cplusplus
}
#endif
#endif
