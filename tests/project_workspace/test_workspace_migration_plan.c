#include "umicom/project/workspace/workspace_migration_plan.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceWorkspaceMigrationPlan v;
    umi_project_workspace_workspace_migration_plan_init(&v);
    CHECK(umi_project_workspace_workspace_migration_plan_add(&v,"scan","Scan source",true)==UMI_STATUS_OK);
    CHECK(!umi_project_workspace_workspace_migration_plan_ready(&v));
    CHECK(umi_project_workspace_workspace_migration_plan_complete(&v,"scan")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_migration_plan_ready(&v));
    return 0;
}
