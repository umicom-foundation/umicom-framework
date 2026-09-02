/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_cmake_install_rule.c
 *
 * PURPOSE:
 *   Implement the test cmake install rule behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/cmake_install_rule.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceCmakeInstallRule a,b;
    CHECK(umi_project_workspace_cmake_install_rule_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_install_rule_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_install_rule_set_name(&a,"Cmake Install Rule")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_install_rule_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_cmake_install_rule_set_metric(&a,7U);
    CHECK(umi_project_workspace_cmake_install_rule_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_cmake_install_rule_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Cmake Install Rule")==0);
    return 0;
}
