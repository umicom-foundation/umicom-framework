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
#include "../value_archive/revision_cases.h"
#include "umicom/project/workspace/workspace_import_plan.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceWorkspaceImportPlan v;
    umi_project_workspace_workspace_import_plan_init(&v);
    CHECK(umi_project_workspace_workspace_import_plan_add(&v,"scan","Scan source",true)==UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiProjectWorkspaceWorkspaceImportPlan, &v,
        (umi_project_workspace_workspace_import_plan_add(&v,"scan","Scan source",true)));

    CHECK(!umi_project_workspace_workspace_import_plan_ready(&v));
    CHECK(umi_project_workspace_workspace_import_plan_complete(&v,"scan")==UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiProjectWorkspaceWorkspaceImportPlan, &v,
        (umi_project_workspace_workspace_import_plan_complete(&v,"scan")));

    CHECK(umi_project_workspace_workspace_import_plan_ready(&v));
    return 0;
}
