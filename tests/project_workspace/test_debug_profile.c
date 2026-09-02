/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_debug_profile.c
 *
 * PURPOSE:
 *   Implement the test debug profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/debug_profile.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceDebugProfile a,b;
    CHECK(umi_project_workspace_debug_profile_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_debug_profile_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_debug_profile_set_name(&a,"Debug Profile")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_debug_profile_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_debug_profile_set_metric(&a,7U);
    CHECK(umi_project_workspace_debug_profile_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_debug_profile_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Debug Profile")==0);
    return 0;
}
