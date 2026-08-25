#include "umicom/project/workspace/application_blueprint.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceApplicationBlueprint a,b;
    CHECK(umi_project_workspace_application_blueprint_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_application_blueprint_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_application_blueprint_set_name(&a,"Application Blueprint")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_application_blueprint_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_application_blueprint_set_metric(&a,7U);
    CHECK(umi_project_workspace_application_blueprint_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_application_blueprint_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Application Blueprint")==0);
    return 0;
}
