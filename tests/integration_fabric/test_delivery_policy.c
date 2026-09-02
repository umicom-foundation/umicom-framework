/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_delivery_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the delivery policy Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/delivery_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricDeliveryPolicy item;
    CHECK(umi_fabric_delivery_policy_init(&item,"orders",UMI_FABRIC_DELIVERY_AT_LEAST_ONCE,3U,5000U,true)==UMI_STATUS_OK);
    CHECK(item.durable);
    return 0;
}
