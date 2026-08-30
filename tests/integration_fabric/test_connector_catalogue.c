/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_connector_catalogue.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the connector catalogue Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/connector_catalogue.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricConnectorCatalogue r; UmiFabricConnectorDescriptor item, found; umi_fabric_connector_catalogue_init(&r);
    CHECK(umi_fabric_connector_descriptor_init(&item,"x","X",UMI_FABRIC_PROTOCOL_HTTP,1U,false)==UMI_STATUS_OK);
    CHECK(umi_fabric_connector_catalogue_add(&r,&item)==UMI_STATUS_OK);
    CHECK(umi_fabric_connector_catalogue_add(&r,&item)==UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_fabric_connector_catalogue_find(&r,item.connector_id,&found)==UMI_STATUS_OK);
    CHECK(umi_fabric_connector_catalogue_count(&r)==1U);
    return 0;
}
