/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_workspace_migration_plan.c
 *
 * PURPOSE:
 *   Implement the test workspace migration plan behavior for
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
#include "umicom/project/workspace/workspace_migration_plan.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceWorkspaceMigrationPlan v;
    umi_project_workspace_workspace_migration_plan_init(&v);
    CHECK(umi_project_workspace_workspace_migration_plan_add(&v,"scan","Scan source",true)==UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiProjectWorkspaceWorkspaceMigrationPlan, &v,
        (umi_project_workspace_workspace_migration_plan_add(&v,"scan","Scan source",true)));

    CHECK(!umi_project_workspace_workspace_migration_plan_ready(&v));
    CHECK(umi_project_workspace_workspace_migration_plan_complete(&v,"scan")==UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiProjectWorkspaceWorkspaceMigrationPlan, &v,
        (umi_project_workspace_workspace_migration_plan_complete(&v,"scan")));

    CHECK(umi_project_workspace_workspace_migration_plan_ready(&v));
    return 0;
}
