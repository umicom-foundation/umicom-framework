/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_workspace_import_plan.c
 *
 * PURPOSE:
 *   Implement the test workspace import plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/workspace_import_plan.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceWorkspaceImportPlan v;
    umi_project_workspace_workspace_import_plan_init(&v);
    CHECK(umi_project_workspace_workspace_import_plan_add(&v,"scan","Scan source",true)==UMI_STATUS_OK);
    CHECK(!umi_project_workspace_workspace_import_plan_ready(&v));
    CHECK(umi_project_workspace_workspace_import_plan_complete(&v,"scan")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_import_plan_ready(&v));
    return 0;
}
