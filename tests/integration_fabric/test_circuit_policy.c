/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_circuit_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the circuit policy Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/circuit_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricCircuitPolicy p; UmiFabricCircuitEvidence e={0}; umi_fabric_circuit_policy_default(&p); CHECK(umi_fabric_circuit_record(&p,&e,false,1U)==UMI_STATUS_OK); CHECK(umi_fabric_circuit_record(&p,&e,false,2U)==UMI_STATUS_OK); CHECK(umi_fabric_circuit_record(&p,&e,false,3U)==UMI_STATUS_OK); CHECK(e.state==UMI_FABRIC_CIRCUIT_OPEN); CHECK(umi_fabric_circuit_record(&p,&e,true,2000U)==UMI_STATUS_OK); CHECK(e.state==UMI_FABRIC_CIRCUIT_HALF_OPEN);
    return 0;
}
