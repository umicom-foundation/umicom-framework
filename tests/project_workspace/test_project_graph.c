#include "umicom/project/workspace/project_graph.h"
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
int main(void) {
    UmiProjectWorkspaceProjectGraph g;
    umi_project_workspace_project_graph_init(&g);
    CHECK(umi_project_workspace_project_graph_add_node(&g,"framework")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_graph_add_node(&g,"studio")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_graph_add_edge(&g,"studio","framework")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_graph_has_edge(&g,"studio","framework"));
    return 0;
}
