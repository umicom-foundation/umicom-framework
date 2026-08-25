/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_transform_pipeline.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the transform pipeline Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/transform_pipeline.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricTransformPlan p; UmiFabricTransformRule r; uint64_t h=0U; umi_fabric_transform_plan_init(&p); CHECK(umi_fabric_transform_rule_init(&r,"r","a","b","copy",true)==UMI_STATUS_OK); CHECK(umi_fabric_transform_plan_add(&p,&r)==UMI_STATUS_OK); CHECK(umi_fabric_transform_pipeline_validate(&p,4U,&h)==UMI_STATUS_OK); CHECK(h!=0U);
    return 0;
}
