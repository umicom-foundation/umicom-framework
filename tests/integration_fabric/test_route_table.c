/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_route_table.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the route table Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/route_table.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricRouteTable r; UmiFabricRouteRule item, found; umi_fabric_route_table_init(&r);
    CHECK(umi_fabric_route_rule_init(&item,"r1","*","trade.*","risk",10U)==UMI_STATUS_OK);
    CHECK(umi_fabric_route_table_add(&r,&item)==UMI_STATUS_OK);
    CHECK(umi_fabric_route_table_add(&r,&item)==UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_fabric_route_table_find(&r,item.route_id,&found)==UMI_STATUS_OK);
    CHECK(umi_fabric_route_table_count(&r)==1U);
    return 0;
}
