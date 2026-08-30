/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_build_profile.c
 *
 * PURPOSE:
 *   Implement the test build profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/build_profile.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceBuildProfile a,b;
    CHECK(umi_project_workspace_build_profile_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_build_profile_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_build_profile_set_name(&a,"Build Profile")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_build_profile_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_build_profile_set_metric(&a,7U);
    CHECK(umi_project_workspace_build_profile_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_build_profile_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Build Profile")==0);
    return 0;
}
