/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_compensation_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the compensation policy Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/compensation_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricSagaPlan p; UmiFabricSagaStep a,b; char ids[UMI_FABRIC_MAX_STEPS][UMI_FABRIC_ID_CAPACITY]={{0}}; size_t n=0U; umi_fabric_saga_plan_init(&p); CHECK(umi_fabric_saga_step_init(&a,"a","doa","undoa",true)==UMI_STATUS_OK); CHECK(umi_fabric_saga_step_init(&b,"b","dob","undob",true)==UMI_STATUS_OK); CHECK(umi_fabric_saga_plan_add(&p,&a)==UMI_STATUS_OK); CHECK(umi_fabric_saga_plan_add(&p,&b)==UMI_STATUS_OK); CHECK(umi_fabric_compensation_order(&p,2U,ids,&n)==UMI_STATUS_OK); CHECK(n==2U&&strcmp(ids[0],"b")==0);
    return 0;
}
