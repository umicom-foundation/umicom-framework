#include "umicom/project/workspace/project_migration.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceProjectMigration a,b;
    CHECK(umi_project_workspace_project_migration_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_migration_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_migration_set_name(&a,"Project Migration")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_migration_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_project_migration_set_metric(&a,7U);
    CHECK(umi_project_workspace_project_migration_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_project_migration_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Project Migration")==0);
    return 0;
}
