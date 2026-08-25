/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_integration_fabric_service.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the integration fabric service Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/integration_fabric_service.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiIntegrationFabricService s; UmiFabricSnapshot snap; umi_integration_fabric_service_init(&s); CHECK(umi_integration_fabric_service_start(&s)==UMI_STATUS_OK); CHECK(umi_integration_fabric_service_snapshot(&s,&snap)==UMI_STATUS_OK); CHECK(snap.health==UMI_FABRIC_STATE_READY);
    return 0;
}
