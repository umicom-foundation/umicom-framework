/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_project_capability.c
 *
 * PURPOSE:
 *   Implement the test project capability behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/project_capability.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceProjectCapability a,b;
    CHECK(umi_project_workspace_project_capability_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_capability_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_capability_set_name(&a,"Project Capability")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_capability_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_project_capability_set_metric(&a,7U);
    CHECK(umi_project_workspace_project_capability_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_project_capability_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Project Capability")==0);
    return 0;
}
