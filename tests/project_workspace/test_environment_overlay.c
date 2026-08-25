#include "umicom/project/workspace/environment_overlay.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceEnvironmentOverlay a,b;
    CHECK(umi_project_workspace_environment_overlay_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_environment_overlay_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_environment_overlay_set_name(&a,"Environment Overlay")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_environment_overlay_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_environment_overlay_set_metric(&a,7U);
    CHECK(umi_project_workspace_environment_overlay_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_environment_overlay_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Environment Overlay")==0);
    return 0;
}
