/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_transform_rule.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the transform rule Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/transform_rule.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricTransformRule item;
    CHECK(umi_fabric_transform_rule_init(&item,"t1","trade.price","risk.price","copy",true)==UMI_STATUS_OK);
    CHECK(item.required);
    return 0;
}
