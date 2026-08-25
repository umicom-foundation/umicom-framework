/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_connector_health.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the connector health Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/connector_health.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricConnectorHealth h; umi_fabric_connector_health_init(&h); umi_fabric_connector_health_record(&h,true,false,true); umi_fabric_connector_health_record(&h,true,true,false); CHECK(h.state==UMI_FABRIC_STATE_DEGRADED); CHECK(umi_fabric_connector_health_error_rate(&h)==50.0);
    return 0;
}
