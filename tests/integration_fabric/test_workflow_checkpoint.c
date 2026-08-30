/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_workflow_checkpoint.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the workflow checkpoint Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/workflow_checkpoint.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricWorkflowPlan p={0}; UmiFabricWorkflowCheckpoint c; p.count=2U;p.fingerprint=9U;CHECK(umi_fabric_workflow_checkpoint_init(&c,"wf",&p,1U)==UMI_STATUS_OK); CHECK(umi_fabric_workflow_checkpoint_advance(&c,&p,2U)==UMI_STATUS_OK); CHECK(c.next_step==1U);
    return 0;
}
