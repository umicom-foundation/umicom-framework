/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_service_registry.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the service registry Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_registry.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricServiceRegistry r; UmiFabricServiceInstance item, found; umi_fabric_service_registry_init(&r);
    CHECK(umi_fabric_service_instance_init(&item,"risk.1","risk","ep.1",1U,10U)==UMI_STATUS_OK);
    CHECK(umi_fabric_service_registry_add(&r,&item)==UMI_STATUS_OK);
    CHECK(umi_fabric_service_registry_add(&r,&item)==UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_fabric_service_registry_find(&r,item.instance_id,&found)==UMI_STATUS_OK);
    CHECK(umi_fabric_service_registry_count(&r)==1U);
    return 0;
}
