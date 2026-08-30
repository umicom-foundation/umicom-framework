/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/conflict_resolution.h
 *
 * PURPOSE:
 *   Track per-hunk conflict choices and completion state for three-way merge workflows.
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
#ifndef UMICOM_VCS_ADVANCED_CONFLICT_RESOLUTION_H
#define UMICOM_VCS_ADVANCED_CONFLICT_RESOLUTION_H
#include "umicom/vcs/advanced/conflict_hunk.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsAdvancedConflictResolution {
    uint32_t struct_size; uint32_t api_version;
    char path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    UmiVcsAdvancedConflictHunk hunks[UMI_VCS_ADVANCED_SMALL_CAPACITY]; size_t hunk_count;
    size_t resolved_count; uint64_t result_fingerprint; UmiVcsAdvancedState state;
} UmiVcsAdvancedConflictResolution;
void umi_vcs_advanced_conflict_resolution_init(UmiVcsAdvancedConflictResolution *r);
UmiStatus umi_vcs_advanced_conflict_resolution_set_path(UmiVcsAdvancedConflictResolution *r,const char *path);
UmiStatus umi_vcs_advanced_conflict_resolution_add(UmiVcsAdvancedConflictResolution *r,const UmiVcsAdvancedConflictHunk *hunk);
UmiStatus umi_vcs_advanced_conflict_resolution_choose(UmiVcsAdvancedConflictResolution *r,size_t index,UmiVcsConflictChoice choice);
int umi_vcs_advanced_conflict_resolution_complete(const UmiVcsAdvancedConflictResolution *r);
#ifdef __cplusplus
}
#endif
#endif
