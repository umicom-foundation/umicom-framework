/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_service_health.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the service health Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_health.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricServiceHealth h; umi_fabric_service_health_init(&h); umi_fabric_service_health_record(&h,false,10U,2U); CHECK(h.state==UMI_FABRIC_STATE_DEGRADED); umi_fabric_service_health_record(&h,false,11U,2U); CHECK(h.state==UMI_FABRIC_STATE_FAILED); umi_fabric_service_health_record(&h,true,12U,2U); CHECK(h.state==UMI_FABRIC_STATE_READY);
    return 0;
}
