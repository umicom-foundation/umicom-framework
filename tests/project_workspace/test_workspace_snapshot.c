/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_workspace_snapshot.c
 *
 * PURPOSE:
 *   Implement the test workspace snapshot behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/workspace_snapshot.h"
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceWorkspaceSnapshot s;
    CHECK(umi_project_workspace_workspace_snapshot_init(&s,"workspace")==UMI_STATUS_OK);
    s.project_count=2U;
    s.ready_project_count=1U;
    umi_project_workspace_workspace_snapshot_recalculate(&s);
    CHECK(s.state==UMI_PROJECT_WORKSPACE_DEGRADED);
    s.blocked_project_count=1U;
    umi_project_workspace_workspace_snapshot_recalculate(&s);
    CHECK(s.state==UMI_PROJECT_WORKSPACE_BLOCKED);
    return 0;
}
