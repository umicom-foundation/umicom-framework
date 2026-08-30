/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_blame_range.c
 *
 * PURPOSE:
 *   Validate represent and normalize line ranges requested from blame/history providers.
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
#include "umicom/vcs/advanced/blame_range.h"

int main(void)
{
    UmiVcsAdvancedBlameRange value;
    umi_vcs_advanced_blame_range_init(&value);
    if (umi_vcs_advanced_blame_range_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_blame_range_set(&value, 10U, 14U) != UMI_STATUS_OK) return 2;
    if (umi_vcs_advanced_blame_range_count(&value) != 5U) return 3;
    return 0;
}
