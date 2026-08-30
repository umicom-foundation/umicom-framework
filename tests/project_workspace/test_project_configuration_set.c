/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_project_configuration_set.c
 *
 * PURPOSE:
 *   Implement the test project configuration set behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/project_configuration_set.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceProjectConfigurationSet v;
    umi_project_workspace_project_configuration_set_init(&v);
    CHECK(umi_project_workspace_project_configuration_set_add(&v,"one")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_configuration_set_contains(&v,"one"));
    CHECK(umi_project_workspace_project_configuration_set_remove(&v,"one")==UMI_STATUS_OK);
    CHECK(!umi_project_workspace_project_configuration_set_contains(&v,"one"));
    return 0;
}
