#include "umicom/project/workspace/project_dependency_cycle.h"
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
int main(void) {
    UmiProjectWorkspaceProjectGraph g;
    umi_project_workspace_project_graph_init(&g);
    CHECK(umi_project_workspace_project_graph_add_node(&g,"a")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_graph_add_node(&g,"b")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_graph_add_edge(&g,"a","b")==UMI_STATUS_OK);
    CHECK(!umi_project_workspace_project_dependency_cycle_detect(&g));
    CHECK(umi_project_workspace_project_graph_add_edge(&g,"b","a")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_dependency_cycle_detect(&g));
    return 0;
}
