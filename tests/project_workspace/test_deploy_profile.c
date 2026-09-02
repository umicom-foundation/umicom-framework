/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_deploy_profile.c
 *
 * PURPOSE:
 *   Implement the test deploy profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/deploy_profile.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceDeployProfile a,b;
    CHECK(umi_project_workspace_deploy_profile_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_deploy_profile_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_deploy_profile_set_name(&a,"Deploy Profile")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_deploy_profile_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_deploy_profile_set_metric(&a,7U);
    CHECK(umi_project_workspace_deploy_profile_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_deploy_profile_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Deploy Profile")==0);
    return 0;
}
