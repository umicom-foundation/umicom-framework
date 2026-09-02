/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_conflict_marker.c
 *
 * PURPOSE:
 *   Validate scan textual merge-marker lines without requiring editor-specific parsing.
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
#include "umicom/vcs/advanced/conflict_marker.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){size_t n=0U;const char *t="<<<<<<< ours\nA\n=======\nB\n>>>>>>> theirs\n";
/* Apply this branch only when its contract condition is satisfied. */
if(umi_vcs_advanced_conflict_marker_classify("<<<<<<< x")!=UMI_VCS_MARKER_OURS)return 1;
/* Apply this branch only when its contract condition is satisfied. */
if(!umi_vcs_advanced_conflict_marker_balanced(t,&n)||n!=1U)return 2;
    return 0;
}
