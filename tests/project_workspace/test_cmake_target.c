/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_cmake_target.c
 *
 * PURPOSE:
 *   Implement the test cmake target behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/cmake_target.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceCmakeTarget a,b;
    CHECK(umi_project_workspace_cmake_target_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_target_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_target_set_name(&a,"Cmake Target")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_target_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_cmake_target_set_metric(&a,7U);
    CHECK(umi_project_workspace_cmake_target_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_cmake_target_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Cmake Target")==0);
    return 0;
}
