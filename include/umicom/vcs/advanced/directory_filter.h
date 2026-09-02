/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/directory_filter.h
 *
 * PURPOSE:
 *   Define deterministic inclusion policy for large directory comparisons.
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
#ifndef UMICOM_VCS_ADVANCED_DIRECTORY_FILTER_H
#define UMICOM_VCS_ADVANCED_DIRECTORY_FILTER_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced directory filter data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedDirectoryFilter {
    uint32_t struct_size;
    uint32_t api_version;
    char extension[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char path_prefix[UMI_VCS_ADVANCED_PATH_CAPACITY];
    uint64_t maximum_size_bytes;
    int include_hidden;
    int include_directories;
    int include_binary;
} UmiVcsAdvancedDirectoryFilter;

/**
 * Initialise vcs advanced directory filter from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_directory_filter_init(UmiVcsAdvancedDirectoryFilter *value);
/**
 * Check that vcs advanced directory filter satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_directory_filter_validate(const UmiVcsAdvancedDirectoryFilter *value);
/**
 * Provide the vcs advanced directory filter accept operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_directory_filter_accept(const UmiVcsAdvancedDirectoryFilter *filter,
                                                const char *relative_path,
                                                uint64_t size_bytes,
                                                int directory,
                                                int hidden,
                                                int binary);

#ifdef __cplusplus
}
#endif

#endif
