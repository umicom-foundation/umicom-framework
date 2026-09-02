/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/directory_diff.h
 *
 * PURPOSE:
 *   Compare directory entries and classify left-only, right-only, changed and type-changed state.
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
#ifndef UMICOM_VCS_ADVANCED_DIRECTORY_DIFF_H
#define UMICOM_VCS_ADVANCED_DIRECTORY_DIFF_H
#include "umicom/vcs/advanced/directory_entry.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs advanced directory diff data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedDirectoryDiff {
    uint32_t struct_size; uint32_t api_version;
    char relative_path[UMI_VCS_ADVANCED_PATH_CAPACITY]; UmiVcsDirectoryState state;
    UmiVcsAdvancedDirectoryEntry left,right; int has_left,has_right;
} UmiVcsAdvancedDirectoryDiff;
/**
 * Provide the vcs advanced directory diff compare operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_directory_diff_compare(const UmiVcsAdvancedDirectoryEntry *left,
                                                    const UmiVcsAdvancedDirectoryEntry *right,
                                                    UmiVcsAdvancedDirectoryDiff *out_diff);
#ifdef __cplusplus
}
#endif
#endif
