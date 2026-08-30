/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_bulkhead_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the bulkhead policy Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/bulkhead_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricBulkheadPolicy p; bool admit=false,q=false; umi_fabric_bulkhead_policy_default(&p); CHECK(umi_fabric_bulkhead_policy_evaluate(&p,16U,10U,&admit,&q)==UMI_STATUS_OK); CHECK(!admit&&q); CHECK(umi_fabric_bulkhead_policy_evaluate(&p,16U,64U,&admit,&q)==UMI_STATUS_OK); CHECK(!q);
    return 0;
}
