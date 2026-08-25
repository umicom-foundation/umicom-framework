/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_idempotency_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the idempotency policy Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/idempotency_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricIdempotencyPolicy p; bool ok=false; umi_fabric_idempotency_policy_default(&p); CHECK(umi_fabric_idempotency_policy_evaluate(&p,true,true,"short",&ok)==UMI_STATUS_OK); CHECK(!ok); CHECK(umi_fabric_idempotency_policy_evaluate(&p,true,true,"12345678",&ok)==UMI_STATUS_OK); CHECK(ok);
    return 0;
}
