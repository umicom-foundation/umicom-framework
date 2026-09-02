/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/stash_entry.h
 *
 * PURPOSE:
 *   Describe one stash entry with base/index/worktree identity and metadata.
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
#ifndef UMICOM_VCS_ADVANCED_STASH_ENTRY_H
#define UMICOM_VCS_ADVANCED_STASH_ENTRY_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced stash entry data shared with callers of this public contract.
 */
typedef struct UmiVcsAdvancedStashEntry {
    uint32_t struct_size;
    uint32_t api_version;
    size_t index;
    char stash_ref[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char base_oid[UMI_VCS_ADVANCED_OID_CAPACITY];
    char message[UMI_VCS_ADVANCED_TEXT_CAPACITY];
    char branch[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    uint64_t timestamp_seconds;
    int includes_untracked;
} UmiVcsAdvancedStashEntry;

/**
 * Initialise vcs advanced stash entry from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_stash_entry_init(UmiVcsAdvancedStashEntry *value);
/**
 * Check that vcs advanced stash entry satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_stash_entry_validate(const UmiVcsAdvancedStashEntry *value);
/**
 * Copy vcs advanced stash entry into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_stash_entry_set(UmiVcsAdvancedStashEntry *value,
                                             size_t index,
                                             const char *stash_ref,
                                             const char *base_oid,
                                             const char *message,
                                             const char *branch);

#ifdef __cplusplus
}
#endif

#endif
