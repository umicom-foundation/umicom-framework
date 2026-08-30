/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_compare_side.c
 *
 * PURPOSE:
 *   Validate describe one immutable or editable side of a professional comparison session.
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
#include "umicom/vcs/advanced/compare_side.h"

int main(void)
{
    UmiVcsAdvancedCompareSide value;
    umi_vcs_advanced_compare_side_init(&value);
    if (umi_vcs_advanced_compare_side_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_compare_side_set(&value, "left.c", "HEAD", "Left", 1) != UMI_STATUS_OK) return 2;
    return 0;
}
