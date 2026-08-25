#include "umicom/project/workspace/cmake_dependency.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceCmakeDependency a,b;
    CHECK(umi_project_workspace_cmake_dependency_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_dependency_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_dependency_set_name(&a,"Cmake Dependency")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_dependency_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_cmake_dependency_set_metric(&a,7U);
    CHECK(umi_project_workspace_cmake_dependency_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_cmake_dependency_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Cmake Dependency")==0);
    return 0;
}
