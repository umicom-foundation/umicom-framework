/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_service_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the service descriptor Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_descriptor.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricServiceDescriptor item;
    CHECK(umi_fabric_service_descriptor_init(&item,"risk","Risk Service",(UmiFabricVersion){1U,2U,0U},7U)==UMI_STATUS_OK);
    CHECK(item.capability_mask==7U);
    return 0;
}
