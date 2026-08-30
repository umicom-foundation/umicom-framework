/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_blame_line.c
 *
 * PURPOSE:
 *   Validate describe line-level provenance without leaking git parser details into applications.
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
#include "umicom/vcs/advanced/blame_line.h"

int main(void)
{
    UmiVcsAdvancedBlameLine value;
    umi_vcs_advanced_blame_line_init(&value);
    if (umi_vcs_advanced_blame_line_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_blame_line_set(&value, 7U, "deadbeef", "Sammy", "src/a.c", "line") != UMI_STATUS_OK) return 2;
    if (value.content_fingerprint == 0U) return 3;
    return 0;
}
