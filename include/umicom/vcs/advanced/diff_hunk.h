/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/diff_hunk.h
 *
 * PURPOSE:
 *   Describe normalized change blocks for navigation and partial operations.
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
#ifndef UMICOM_VCS_ADVANCED_DIFF_HUNK_H
#define UMICOM_VCS_ADVANCED_DIFF_HUNK_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced diff hunk data shared with callers of this public contract.
 */
typedef struct UmiVcsAdvancedDiffHunk {
    uint32_t struct_size;
    uint32_t api_version;
    size_t old_start;
    size_t old_count;
    size_t new_start;
    size_t new_count;
    size_t added_lines;
    size_t deleted_lines;
    size_t modified_lines;
    uint64_t fingerprint;
} UmiVcsAdvancedDiffHunk;

/**
 * Initialise vcs advanced diff hunk from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_diff_hunk_init(UmiVcsAdvancedDiffHunk *value);
/**
 * Check that vcs advanced diff hunk satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_diff_hunk_validate(const UmiVcsAdvancedDiffHunk *value);
/**
 * Provide the vcs advanced diff hunk set counts operation used by this module and its
 * client applications.
 */
void umi_vcs_advanced_diff_hunk_set_counts(UmiVcsAdvancedDiffHunk *value,
                                             size_t added,
                                             size_t deleted,
                                             size_t modified);
/**
 * Return the number of records represented by vcs advanced diff hunk change without
 * changing their state.
 */
size_t umi_vcs_advanced_diff_hunk_change_count(const UmiVcsAdvancedDiffHunk *value);

#ifdef __cplusplus
}
#endif

#endif
