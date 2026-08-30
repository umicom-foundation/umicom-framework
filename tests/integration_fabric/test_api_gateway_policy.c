/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_api_gateway_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the api gateway policy Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/api_gateway_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricApiGatewayPolicy p; UmiFabricApiOperation op; bool ok=false; umi_fabric_api_gateway_policy_default(&p); CHECK(umi_fabric_api_operation_init(&op,"post","POST","/x","x","x",false)==UMI_STATUS_OK); CHECK(umi_fabric_api_gateway_policy_evaluate(&p,&op,true,true,10U,&ok)==UMI_STATUS_OK); CHECK(!ok);
    return 0;
}
