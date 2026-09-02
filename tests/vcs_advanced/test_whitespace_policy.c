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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/whitespace_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){/* Use the stable identifier comparison to choose the matching record or policy. */ if(!umi_vcs_advanced_whitespace_equal("a   b","a b",UMI_VCS_WHITESPACE_IGNORE_AMOUNT))return 1;
/* Use the stable identifier comparison to choose the matching record or policy. */
if(umi_vcs_advanced_whitespace_equal("a b","ab",UMI_VCS_WHITESPACE_IGNORE_AMOUNT))return 2;
    return 0;
}
