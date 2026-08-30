/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_workflow_step.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the workflow step Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/workflow_step.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricWorkflowStep item;
    CHECK(umi_fabric_workflow_step_init(&item,"reserve","inventory.reserve",1000U,false,true)==UMI_STATUS_OK);
    CHECK(item.compensatable);
    return 0;
}
