/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_directory_filter.c
 *
 * PURPOSE:
 *   Validate define deterministic inclusion policy for large directory comparisons.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/directory_filter.h"

int main(void)
{
    UmiVcsAdvancedDirectoryFilter value;
    umi_vcs_advanced_directory_filter_init(&value);
    if (umi_vcs_advanced_directory_filter_validate(&value) != UMI_STATUS_OK) return 2;
    if (!umi_vcs_advanced_directory_filter_accept(&value, "src/a.c", 12U, 0, 0, 0)) return 3;
    return 0;
}
