/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_endpoint_registry.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the endpoint registry Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/endpoint_registry.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricEndpointRegistry r; UmiFabricEndpointDescriptor item, found; umi_fabric_endpoint_registry_init(&r);
    CHECK(umi_fabric_endpoint_descriptor_init(&item,"e1","https://risk",UMI_FABRIC_PROTOCOL_HTTP,true,10U)==UMI_STATUS_OK);
    CHECK(umi_fabric_endpoint_registry_add(&r,&item)==UMI_STATUS_OK);
    CHECK(umi_fabric_endpoint_registry_add(&r,&item)==UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_fabric_endpoint_registry_find(&r,item.endpoint_id,&found)==UMI_STATUS_OK);
    CHECK(umi_fabric_endpoint_registry_count(&r)==1U);
    return 0;
}
