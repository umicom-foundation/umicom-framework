/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_workflow_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the workflow plan Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/workflow_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricWorkflowGraph g; UmiFabricWorkflowStep a,b; UmiFabricWorkflowPlan p; umi_fabric_workflow_graph_init(&g); CHECK(umi_fabric_workflow_step_init(&a,"a","op.a",100U,false,false)==UMI_STATUS_OK); CHECK(umi_fabric_workflow_step_init(&b,"b","op.b",100U,false,false)==UMI_STATUS_OK); CHECK(umi_fabric_workflow_graph_add_step(&g,&b)==UMI_STATUS_OK); CHECK(umi_fabric_workflow_graph_add_step(&g,&a)==UMI_STATUS_OK); CHECK(umi_fabric_workflow_graph_add_edge(&g,"a","b")==UMI_STATUS_OK); CHECK(umi_fabric_workflow_plan_build(&g,&p)==UMI_STATUS_OK); CHECK(strcmp(p.step_ids[0],"a")==0);
    return 0;
}
