/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/staging_hunk.h
 *
 * PURPOSE:
 *   Describe a selectable diff hunk for partial staging and unstaging.
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
#ifndef UMICOM_VCS_ADVANCED_STAGING_HUNK_H
#define UMICOM_VCS_ADVANCED_STAGING_HUNK_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced staging hunk data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedStagingHunk {
    uint32_t struct_size;
    uint32_t api_version;
    char path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    size_t old_start;
    size_t old_count;
    size_t new_start;
    size_t new_count;
    size_t added_lines;
    size_t deleted_lines;
    uint64_t fingerprint;
    int selected;
} UmiVcsAdvancedStagingHunk;

/**
 * Initialise vcs advanced staging hunk from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_staging_hunk_init(UmiVcsAdvancedStagingHunk *value);
/**
 * Check that vcs advanced staging hunk satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_staging_hunk_validate(const UmiVcsAdvancedStagingHunk *value);
/**
 * Copy vcs advanced staging hunk into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_staging_hunk_set(UmiVcsAdvancedStagingHunk *value,
                                              const char *path,
                                              size_t old_start,
                                              size_t old_count,
                                              size_t new_start,
                                              size_t new_count,
                                              const char *patch_text);

#ifdef __cplusplus
}
#endif

#endif
