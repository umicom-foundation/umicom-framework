/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/conflict_hunk.h
 *
 * PURPOSE:
 *   Describe aligned base/ours/theirs ranges for one textual conflict.
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
#ifndef UMICOM_VCS_ADVANCED_CONFLICT_HUNK_H
#define UMICOM_VCS_ADVANCED_CONFLICT_HUNK_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced conflict hunk data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedConflictHunk {
    uint32_t struct_size;
    uint32_t api_version;
    size_t base_start;
    size_t base_count;
    size_t ours_start;
    size_t ours_count;
    size_t theirs_start;
    size_t theirs_count;
    uint64_t fingerprint;
    UmiVcsConflictChoice choice;
} UmiVcsAdvancedConflictHunk;

/**
 * Initialise vcs advanced conflict hunk from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_conflict_hunk_init(UmiVcsAdvancedConflictHunk *value);
/**
 * Check that vcs advanced conflict hunk satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_conflict_hunk_validate(const UmiVcsAdvancedConflictHunk *value);
/**
 * Copy vcs advanced conflict hunk into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_conflict_hunk_set(UmiVcsAdvancedConflictHunk *value,
                                               size_t base_start,
                                               size_t base_count,
                                               size_t ours_start,
                                               size_t ours_count,
                                               size_t theirs_start,
                                               size_t theirs_count,
                                               const char *identity_text);

#ifdef __cplusplus
}
#endif

#endif
