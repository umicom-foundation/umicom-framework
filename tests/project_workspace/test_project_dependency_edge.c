/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_project_dependency_edge.c
 *
 * PURPOSE:
 *   Implement the test project dependency edge behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/project_dependency_edge.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceProjectDependencyEdge a,b;
    CHECK(umi_project_workspace_project_dependency_edge_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_dependency_edge_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_dependency_edge_set_name(&a,"Project Dependency Edge")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_dependency_edge_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_project_dependency_edge_set_metric(&a,7U);
    CHECK(umi_project_workspace_project_dependency_edge_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_project_dependency_edge_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Project Dependency Edge")==0);
    return 0;
}
