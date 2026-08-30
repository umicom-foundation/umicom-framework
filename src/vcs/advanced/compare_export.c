/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/compare_export.c
 *
 * PURPOSE:
 *   Implement a safe export request for reports, patches and merge results.
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
#include "umicom/vcs/advanced/compare_export.h"

#include <string.h>

void umi_vcs_advanced_compare_export_init(UmiVcsAdvancedCompareExport *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_compare_export_validate(const UmiVcsAdvancedCompareExport *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->destination) || value->format > 3U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_compare_export_set(UmiVcsAdvancedCompareExport *value,
                                                const char *destination,
                                                UmiVcsCompareExportFormat format)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->destination, sizeof(value->destination), destination);
    if (status != UMI_STATUS_OK) return status;
    value->format = (uint32_t)format;
    value->include_context = 1;
    value->include_metadata = 1;
    return umi_vcs_advanced_compare_export_validate(value);
}
