/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/compare_export.h
 *
 * PURPOSE:
 *   Describe a safe export request for reports, patches and merge results.
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
#ifndef UMICOM_VCS_ADVANCED_COMPARE_EXPORT_H
#define UMICOM_VCS_ADVANCED_COMPARE_EXPORT_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced compare export data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedCompareExport {
    uint32_t struct_size;
    uint32_t api_version;
    char destination[UMI_VCS_ADVANCED_PATH_CAPACITY];
    char title[UMI_VCS_ADVANCED_TEXT_CAPACITY];
    uint32_t format;
    int include_context;
    int include_metadata;
    int overwrite;
} UmiVcsAdvancedCompareExport;

/**
 * Initialise vcs advanced compare export from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_compare_export_init(UmiVcsAdvancedCompareExport *value);
/**
 * Check that vcs advanced compare export satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_compare_export_validate(const UmiVcsAdvancedCompareExport *value);
/**
 * List the named vcs compare export format values accepted by this public contract.
 */
typedef enum UmiVcsCompareExportFormat {
    UMI_VCS_COMPARE_EXPORT_UNIFIED_PATCH = 0,
    UMI_VCS_COMPARE_EXPORT_TEXT_REPORT = 1,
    UMI_VCS_COMPARE_EXPORT_JSON = 2,
    UMI_VCS_COMPARE_EXPORT_MERGED_TEXT = 3
} UmiVcsCompareExportFormat;
/**
 * Copy vcs advanced compare export into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_compare_export_set(UmiVcsAdvancedCompareExport *value,
                                                const char *destination,
                                                UmiVcsCompareExportFormat format);

#ifdef __cplusplus
}
#endif

#endif
