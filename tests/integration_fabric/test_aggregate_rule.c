/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_aggregate_rule.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the aggregate rule Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/aggregate_rule.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricAggregateRule item;
    CHECK(umi_fabric_aggregate_rule_init(&item,"a","order_id",4U,1000U,false)==UMI_STATUS_OK);
    CHECK(item.expected_count==4U);
    return 0;
}
