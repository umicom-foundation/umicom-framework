#include "umicom/project/workspace/workspace_policy.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceWorkspacePolicy a,b;
    CHECK(umi_project_workspace_workspace_policy_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_policy_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_policy_set_name(&a,"Workspace Policy")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_policy_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_workspace_policy_set_metric(&a,7U);
    CHECK(umi_project_workspace_workspace_policy_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_workspace_policy_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Workspace Policy")==0);
    return 0;
}
