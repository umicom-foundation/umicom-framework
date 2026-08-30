/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_transform_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the transform plan Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/transform_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricTransformPlan r; UmiFabricTransformRule item, found; umi_fabric_transform_plan_init(&r);
    CHECK(umi_fabric_transform_rule_init(&item,"t1","a","b","copy",true)==UMI_STATUS_OK);
    CHECK(umi_fabric_transform_plan_add(&r,&item)==UMI_STATUS_OK);
    CHECK(umi_fabric_transform_plan_add(&r,&item)==UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_fabric_transform_plan_find(&r,item.rule_id,&found)==UMI_STATUS_OK);
    CHECK(umi_fabric_transform_plan_count(&r)==1U);
    return 0;
}
