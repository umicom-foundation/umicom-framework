/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_workspace_root.c
 *
 * PURPOSE:
 *   Implement the test workspace root behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/workspace_root.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceWorkspaceRoot a,b;
    CHECK(umi_project_workspace_workspace_root_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_root_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_root_set_name(&a,"Workspace Root")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_root_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_workspace_root_set_metric(&a,7U);
    CHECK(umi_project_workspace_workspace_root_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_workspace_root_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Workspace Root")==0);
    return 0;
}
