/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_compare_export.c
 *
 * PURPOSE:
 *   Validate describe a safe export request for reports, patches and merge results.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/compare_export.h"

int main(void)
{
    UmiVcsAdvancedCompareExport value;
    umi_vcs_advanced_compare_export_init(&value);
    if (umi_vcs_advanced_compare_export_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_compare_export_set(&value, "review.patch", UMI_VCS_COMPARE_EXPORT_UNIFIED_PATCH) != UMI_STATUS_OK) return 2;
    return 0;
}
