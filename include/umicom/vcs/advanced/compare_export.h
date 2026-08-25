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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_COMPARE_EXPORT_H
#define UMICOM_VCS_ADVANCED_COMPARE_EXPORT_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_vcs_advanced_compare_export_init(UmiVcsAdvancedCompareExport *value);
UmiStatus umi_vcs_advanced_compare_export_validate(const UmiVcsAdvancedCompareExport *value);
typedef enum UmiVcsCompareExportFormat {
    UMI_VCS_COMPARE_EXPORT_UNIFIED_PATCH = 0,
    UMI_VCS_COMPARE_EXPORT_TEXT_REPORT = 1,
    UMI_VCS_COMPARE_EXPORT_JSON = 2,
    UMI_VCS_COMPARE_EXPORT_MERGED_TEXT = 3
} UmiVcsCompareExportFormat;
UmiStatus umi_vcs_advanced_compare_export_set(UmiVcsAdvancedCompareExport *value,
                                                const char *destination,
                                                UmiVcsCompareExportFormat format);

#ifdef __cplusplus
}
#endif

#endif
