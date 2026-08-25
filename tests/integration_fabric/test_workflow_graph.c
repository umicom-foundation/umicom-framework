/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_workflow_graph.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the workflow graph Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/workflow_graph.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricWorkflowGraph g; UmiFabricWorkflowStep a,b; umi_fabric_workflow_graph_init(&g); CHECK(umi_fabric_workflow_step_init(&a,"a","op.a",100U,false,true)==UMI_STATUS_OK); CHECK(umi_fabric_workflow_step_init(&b,"b","op.b",100U,false,true)==UMI_STATUS_OK); CHECK(umi_fabric_workflow_graph_add_step(&g,&a)==UMI_STATUS_OK); CHECK(umi_fabric_workflow_graph_add_step(&g,&b)==UMI_STATUS_OK); CHECK(umi_fabric_workflow_graph_add_edge(&g,"a","b")==UMI_STATUS_OK); CHECK(umi_fabric_workflow_graph_add_edge(&g,"b","a")==UMI_STATUS_INVALID_STATE);
    return 0;
}
