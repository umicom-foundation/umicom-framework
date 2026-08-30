/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_workspace_health.c
 *
 * PURPOSE:
 *   Implement the test workspace health behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/workspace_health.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceWorkspaceHealth a,b;
    CHECK(umi_project_workspace_workspace_health_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_health_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_health_set_name(&a,"Workspace Health")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_health_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_workspace_health_set_metric(&a,7U);
    CHECK(umi_project_workspace_workspace_health_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_workspace_health_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Workspace Health")==0);
    return 0;
}
