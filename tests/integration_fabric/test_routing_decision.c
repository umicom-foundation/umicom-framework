/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_routing_decision.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the routing decision Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/routing_decision.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricRouteRule r; UmiFabricRoutingDecision d; CHECK(umi_fabric_route_rule_init(&r,"r","*","*","risk",1U)==UMI_STATUS_OK); CHECK(umi_fabric_routing_decision_init(&d,&r,"matched",10U,false)==UMI_STATUS_OK); CHECK(strcmp(d.destination_id,"risk")==0);
    return 0;
}
