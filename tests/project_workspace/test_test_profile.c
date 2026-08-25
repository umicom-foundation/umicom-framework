#include "umicom/project/workspace/test_profile.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceTestProfile a,b;
    CHECK(umi_project_workspace_test_profile_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_test_profile_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_test_profile_set_name(&a,"Test Profile")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_test_profile_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_test_profile_set_metric(&a,7U);
    CHECK(umi_project_workspace_test_profile_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_test_profile_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Test Profile")==0);
    return 0;
}
