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

/*
 * Initialise vcs advanced compare export from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_compare_export_init(UmiVcsAdvancedCompareExport *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

/*
 * Check that vcs advanced compare export satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_compare_export_validate(const UmiVcsAdvancedCompareExport *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->destination) || value->format > 3U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Copy vcs advanced compare export into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_compare_export_set(UmiVcsAdvancedCompareExport *value,
                                                const char *destination,
                                                UmiVcsCompareExportFormat format)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->destination, sizeof(value->destination), destination);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->format = (uint32_t)format;
    value->include_context = 1;
    value->include_metadata = 1;
    return umi_vcs_advanced_compare_export_validate(value);
}
