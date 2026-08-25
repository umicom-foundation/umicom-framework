#include "umicom/project/workspace/cmake_source_group.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceCmakeSourceGroup v;
    umi_project_workspace_cmake_source_group_init(&v);
    CHECK(umi_project_workspace_cmake_source_group_add(&v,"one")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_source_group_contains(&v,"one"));
    CHECK(umi_project_workspace_cmake_source_group_remove(&v,"one")==UMI_STATUS_OK);
    CHECK(!umi_project_workspace_cmake_source_group_contains(&v,"one"));
    return 0;
}
