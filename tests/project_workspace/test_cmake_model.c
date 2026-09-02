/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_cmake_model.c
 *
 * PURPOSE:
 *   Implement the test cmake model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/cmake_model.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceCmakeModel a,b;
    CHECK(umi_project_workspace_cmake_model_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_model_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_model_set_name(&a,"Cmake Model")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_model_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_cmake_model_set_metric(&a,7U);
    CHECK(umi_project_workspace_cmake_model_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_cmake_model_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Cmake Model")==0);
    return 0;
}
