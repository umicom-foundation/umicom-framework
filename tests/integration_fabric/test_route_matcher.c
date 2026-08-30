/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_route_matcher.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the route matcher Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/route_matcher.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricRouteTable t; UmiFabricRouteRule a,b,out; umi_fabric_route_table_init(&t); CHECK(umi_fabric_route_rule_init(&a,"a","trader.*","trade.*","risk",1U)==UMI_STATUS_OK); CHECK(umi_fabric_route_rule_init(&b,"b","trader.*","trade.*","audit",9U)==UMI_STATUS_OK); CHECK(umi_fabric_route_table_add(&t,&a)==UMI_STATUS_OK); CHECK(umi_fabric_route_table_add(&t,&b)==UMI_STATUS_OK); CHECK(umi_fabric_route_match(&t,"trader.gui","trade.new",&out)==UMI_STATUS_OK); CHECK(strcmp(out.destination_id,"audit")==0);
    return 0;
}
