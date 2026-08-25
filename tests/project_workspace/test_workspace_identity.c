#include "umicom/project/workspace/workspace_identity.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceWorkspaceIdentity a,b;
    CHECK(umi_project_workspace_workspace_identity_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_identity_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_identity_set_name(&a,"Workspace Identity")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_identity_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_workspace_identity_set_metric(&a,7U);
    CHECK(umi_project_workspace_workspace_identity_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_workspace_identity_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Workspace Identity")==0);
    return 0;
}
