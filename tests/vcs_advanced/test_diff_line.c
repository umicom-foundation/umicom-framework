/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_diff_line.c
 *
 * PURPOSE:
 *   Validate represent a normalized line in a side-by-side or inline comparison.
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
#include "umicom/vcs/advanced/diff_line.h"

int main(void)
{
    UmiVcsAdvancedDiffLine value;
    umi_vcs_advanced_diff_line_init(&value);
    if (umi_vcs_advanced_diff_line_set(&value, UMI_VCS_DIFF_ADDED, 0U, 2U, "new") != UMI_STATUS_OK) return 2;
    return 0;
}
