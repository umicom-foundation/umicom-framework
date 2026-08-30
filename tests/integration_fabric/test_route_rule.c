/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_route_rule.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the route rule Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/route_rule.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricRouteRule item;
    CHECK(umi_fabric_route_rule_init(&item,"r1","trader.*","trade.*","risk",10U)==UMI_STATUS_OK);
    CHECK(item.enabled);
    return 0;
}
