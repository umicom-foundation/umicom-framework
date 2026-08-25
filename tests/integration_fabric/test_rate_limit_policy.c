/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_rate_limit_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the rate limit policy Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/rate_limit_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricRateLimitPolicy p; bool ok=false; uint32_t remain=0U; umi_fabric_rate_limit_policy_default(&p); CHECK(umi_fabric_rate_limit_policy_evaluate(&p,99U,&ok,&remain)==UMI_STATUS_OK); CHECK(ok&&remain==1U); CHECK(umi_fabric_rate_limit_policy_evaluate(&p,100U,&ok,&remain)==UMI_STATUS_OK); CHECK(!ok);
    return 0;
}
