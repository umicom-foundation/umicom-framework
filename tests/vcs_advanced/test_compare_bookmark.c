/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_compare_bookmark.c
 *
 * PURPOSE:
 *   Validate persist named source locations inside long comparison sessions.
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
#include "umicom/vcs/advanced/compare_bookmark.h"

int main(void)
{
    UmiVcsAdvancedCompareBookmark value;
    umi_vcs_advanced_compare_bookmark_init(&value);
    if (umi_vcs_advanced_compare_bookmark_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_compare_bookmark_set(&value, "a.c", "review", 10U, 2U) != UMI_STATUS_OK) return 2;
    return 0;
}
