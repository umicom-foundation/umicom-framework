/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_environment_profile.c
 *
 * PURPOSE:
 *   Implement the test environment profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/environment_profile.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceEnvironmentProfile a,b;
    CHECK(umi_project_workspace_environment_profile_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_environment_profile_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_environment_profile_set_name(&a,"Environment Profile")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_environment_profile_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_environment_profile_set_metric(&a,7U);
    CHECK(umi_project_workspace_environment_profile_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_environment_profile_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Environment Profile")==0);
    return 0;
}
