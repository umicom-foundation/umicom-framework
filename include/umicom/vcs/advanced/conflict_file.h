/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/conflict_file.h
 *
 * PURPOSE:
 *   Capture index-stage availability and resolution state for a conflicted path.
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
#ifndef UMICOM_VCS_ADVANCED_CONFLICT_FILE_H
#define UMICOM_VCS_ADVANCED_CONFLICT_FILE_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedConflictFile {
    uint32_t struct_size;
    uint32_t api_version;
    char path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    int has_base;
    int has_ours;
    int has_theirs;
    int binary;
    UmiVcsConflictChoice choice;
    UmiVcsAdvancedState state;
} UmiVcsAdvancedConflictFile;

void umi_vcs_advanced_conflict_file_init(UmiVcsAdvancedConflictFile *value);
UmiStatus umi_vcs_advanced_conflict_file_validate(const UmiVcsAdvancedConflictFile *value);
UmiStatus umi_vcs_advanced_conflict_file_set(UmiVcsAdvancedConflictFile *value,
                                               const char *path,
                                               int has_base,
                                               int has_ours,
                                               int has_theirs);

#ifdef __cplusplus
}
#endif

#endif
