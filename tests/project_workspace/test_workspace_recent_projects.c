/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_workspace_recent_projects.c
 *
 * PURPOSE:
 *   Implement the test workspace recent projects behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/workspace_recent_projects.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceWorkspaceRecentProjects v;
    umi_project_workspace_workspace_recent_projects_init(&v);
    CHECK(umi_project_workspace_workspace_recent_projects_add(&v,"one")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_recent_projects_contains(&v,"one"));
    CHECK(umi_project_workspace_workspace_recent_projects_remove(&v,"one")==UMI_STATUS_OK);
    CHECK(!umi_project_workspace_workspace_recent_projects_contains(&v,"one"));
    return 0;
}
