/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_service_instance.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the service instance Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_instance.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricServiceInstance item;
    CHECK(umi_fabric_service_instance_init(&item,"risk.1","risk","ep.1",1U,10U)==UMI_STATUS_OK);
    CHECK(item.healthy);
    return 0;
}
