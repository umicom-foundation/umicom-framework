/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_profile_selection.c
 *
 * PURPOSE:
 *   Implement the test profile selection behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/profile_selection.h"
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceProfileCandidate c[2]= {
        0
    };
    size_t i=0U;
    c[0].capabilities=3U;
    c[0].priority=1U;
    c[0].enabled=true;
    c[1].capabilities=3U;
    c[1].priority=5U;
    c[1].enabled=true;
    CHECK(umi_project_workspace_profile_selection_best(c,2U,1U,&i)==UMI_STATUS_OK);
    CHECK(i==1U);
    return 0;
}
