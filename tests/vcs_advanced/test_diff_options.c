/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_diff_options.c
 *
 * PURPOSE:
 *   Validate define deterministic user comparison options shared by every frontend.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/diff_options.h"

int main(void)
{
    UmiVcsAdvancedDiffOptions value;
    umi_vcs_advanced_diff_options_init(&value);
    if (umi_vcs_advanced_diff_options_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_vcs_advanced_diff_options_fingerprint(&value) == 0U) return 3;
    return 0;
}
