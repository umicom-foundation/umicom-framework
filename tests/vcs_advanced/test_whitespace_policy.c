/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_whitespace_policy.c
 *
 * PURPOSE:
 *   Validate normalize text under explicit whitespace policy for reliable comparisons.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/whitespace_policy.h"
int main(void){if(!umi_vcs_advanced_whitespace_equal("a   b","a b",UMI_VCS_WHITESPACE_IGNORE_AMOUNT))return 1;
if(umi_vcs_advanced_whitespace_equal("a b","ab",UMI_VCS_WHITESPACE_IGNORE_AMOUNT))return 2;
    return 0;
}
