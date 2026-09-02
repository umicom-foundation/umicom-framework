/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_project_dependency_cycle.c
 *
 * PURPOSE:
 *   Implement the test project dependency cycle behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/project_dependency_cycle.h"
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceProjectGraph g;
    umi_project_workspace_project_graph_init(&g);
    CHECK(umi_project_workspace_project_graph_add_node(&g,"a")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_graph_add_node(&g,"b")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_graph_add_edge(&g,"a","b")==UMI_STATUS_OK);
    CHECK(!umi_project_workspace_project_dependency_cycle_detect(&g));
    CHECK(umi_project_workspace_project_graph_add_edge(&g,"b","a")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_dependency_cycle_detect(&g));
    return 0;
}
