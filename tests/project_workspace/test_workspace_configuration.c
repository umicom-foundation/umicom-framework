#include "umicom/project/workspace/workspace_configuration.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceWorkspaceConfiguration a,b;
    CHECK(umi_project_workspace_workspace_configuration_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_configuration_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_configuration_set_name(&a,"Workspace Configuration")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_configuration_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_workspace_configuration_set_metric(&a,7U);
    CHECK(umi_project_workspace_workspace_configuration_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_workspace_configuration_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Workspace Configuration")==0);
    return 0;
}
