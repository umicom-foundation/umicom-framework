/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_project_ordering.c
 *
 * PURPOSE:
 *   Implement the test project ordering behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/project_ordering.h"
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
int main(void) {
    UmiProjectWorkspaceProjectGraph g;
    size_t o[4],n=0U;
    umi_project_workspace_project_graph_init(&g);
    CHECK(umi_project_workspace_project_graph_add_node(&g,"framework")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_graph_add_node(&g,"studio")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_graph_add_edge(&g,"framework","studio")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_ordering_topological(&g,o,4U,&n)==UMI_STATUS_OK);
    CHECK(n==2U&&o[0]==0U&&o[1]==1U);
    return 0;
}
