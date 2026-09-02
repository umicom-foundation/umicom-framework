/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_service_selection.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the service selection Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_selection.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricServiceRegistry r; UmiFabricServiceInstance a,b,out; umi_fabric_service_registry_init(&r);
    CHECK(umi_fabric_service_instance_init(&a,"risk.1","risk","e1",2U,10U)==UMI_STATUS_OK);
    CHECK(umi_fabric_service_instance_init(&b,"risk.2","risk","e2",1U,10U)==UMI_STATUS_OK);
    CHECK(umi_fabric_service_registry_add(&r,&a)==UMI_STATUS_OK); CHECK(umi_fabric_service_registry_add(&r,&b)==UMI_STATUS_OK);
    CHECK(umi_fabric_service_select(&r,"risk",&out)==UMI_STATUS_OK); CHECK(strcmp(out.instance_id,"risk.2")==0);
    return 0;
}
