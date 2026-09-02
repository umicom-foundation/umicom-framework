/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_binary_compare.c
 *
 * PURPOSE:
 *   Validate compare binary artifacts using size and deterministic content fingerprints.
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
#include "umicom/vcs/advanced/binary_compare.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiVcsAdvancedBinaryCompare r;
    umi_vcs_advanced_binary_compare_values(5U,1U,5U,1U,&r);
/* Apply this branch only when its contract condition is satisfied. */
if(!r.same_size||!r.same_content)return 1;
    umi_vcs_advanced_binary_compare_values(5U,1U,6U,1U,&r);
    /* Apply this branch only when its contract condition is satisfied. */
    if (r.same_content)return 2;
    return 0;
}
