/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_workflow_execution.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the workflow execution Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/workflow_execution.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricWorkflowExecution e; CHECK(umi_fabric_workflow_execution_init(&e,"wf")==UMI_STATUS_OK); CHECK(umi_fabric_workflow_execution_transition(&e,UMI_FABRIC_WORKFLOW_RUNNING)==UMI_STATUS_OK); CHECK(umi_fabric_workflow_execution_transition(&e,UMI_FABRIC_WORKFLOW_FAILED)==UMI_STATUS_OK); CHECK(umi_fabric_workflow_execution_transition(&e,UMI_FABRIC_WORKFLOW_COMPENSATING)==UMI_STATUS_OK);
    return 0;
}
