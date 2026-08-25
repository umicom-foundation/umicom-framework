#include "umicom/project/workspace/package_profile.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspacePackageProfile a,b;
    CHECK(umi_project_workspace_package_profile_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_package_profile_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_package_profile_set_name(&a,"Package Profile")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_package_profile_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_package_profile_set_metric(&a,7U);
    CHECK(umi_project_workspace_package_profile_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_package_profile_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Package Profile")==0);
    return 0;
}
