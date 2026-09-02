/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/staging_selection.h
 *
 * PURPOSE:
 *   Represent file, hunk or line scope for partial staging operations.
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
#ifndef UMICOM_VCS_ADVANCED_STAGING_SELECTION_H
#define UMICOM_VCS_ADVANCED_STAGING_SELECTION_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced staging selection data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedStagingSelection {
    uint32_t struct_size;
    uint32_t api_version;
    char path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    size_t hunk_index;
    size_t first_line;
    size_t last_line;
    int entire_file;
    int staged_side;
} UmiVcsAdvancedStagingSelection;

/**
 * Initialise vcs advanced staging selection from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_advanced_staging_selection_init(UmiVcsAdvancedStagingSelection *value);
/**
 * Check that vcs advanced staging selection satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_staging_selection_validate(const UmiVcsAdvancedStagingSelection *value);
/**
 * Provide the vcs advanced staging selection file operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_staging_selection_file(UmiVcsAdvancedStagingSelection *value,
                                                    const char *path,
                                                    int staged_side);
/**
 * Provide the vcs advanced staging selection lines operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_staging_selection_lines(UmiVcsAdvancedStagingSelection *value,
                                                     const char *path,
                                                     size_t hunk_index,
                                                     size_t first_line,
                                                     size_t last_line);

#ifdef __cplusplus
}
#endif

#endif
