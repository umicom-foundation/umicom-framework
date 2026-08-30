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

void umi_vcs_advanced_staging_hunk_init(UmiVcsAdvancedStagingHunk *value);
UmiStatus umi_vcs_advanced_staging_hunk_validate(const UmiVcsAdvancedStagingHunk *value);
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
