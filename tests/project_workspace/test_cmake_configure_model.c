#include "umicom/project/workspace/cmake_configure_model.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceCmakeConfigureModel a,b;
    CHECK(umi_project_workspace_cmake_configure_model_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_configure_model_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_configure_model_set_name(&a,"Cmake Configure Model")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_configure_model_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_cmake_configure_model_set_metric(&a,7U);
    CHECK(umi_project_workspace_cmake_configure_model_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_cmake_configure_model_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Cmake Configure Model")==0);
    return 0;
}
