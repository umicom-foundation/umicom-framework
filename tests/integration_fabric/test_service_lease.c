/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_service_lease.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the service lease Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_lease.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricServiceLease l; CHECK(umi_fabric_service_lease_init(&l,"risk.1",100U,50U)==UMI_STATUS_OK);
    CHECK(umi_fabric_service_lease_active(&l,149U)); CHECK(!umi_fabric_service_lease_active(&l,150U));
    CHECK(umi_fabric_service_lease_renew(&l,200U,20U)==UMI_STATUS_OK); CHECK(l.renewals==1U);
    return 0;
}
