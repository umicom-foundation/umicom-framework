/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_saga_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the saga plan Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/saga_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricSagaPlan r; UmiFabricSagaStep item, found; umi_fabric_saga_plan_init(&r);
    CHECK(umi_fabric_saga_step_init(&item,"a","do","undo",true)==UMI_STATUS_OK);
    CHECK(umi_fabric_saga_plan_add(&r,&item)==UMI_STATUS_OK);
    CHECK(umi_fabric_saga_plan_add(&r,&item)==UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_fabric_saga_plan_find(&r,item.step_id,&found)==UMI_STATUS_OK);
    CHECK(umi_fabric_saga_plan_count(&r)==1U);
    return 0;
}
