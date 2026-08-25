/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_service_catalogue.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the service catalogue Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_catalogue.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricServiceCatalogue r; UmiFabricServiceDescriptor item, found; umi_fabric_service_catalogue_init(&r);
    CHECK(umi_fabric_service_descriptor_init(&item,"risk","Risk",(UmiFabricVersion){1U,0U,0U},1U)==UMI_STATUS_OK);
    CHECK(umi_fabric_service_catalogue_add(&r,&item)==UMI_STATUS_OK);
    CHECK(umi_fabric_service_catalogue_add(&r,&item)==UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_fabric_service_catalogue_find(&r,item.service_id,&found)==UMI_STATUS_OK);
    CHECK(umi_fabric_service_catalogue_count(&r)==1U);
    return 0;
}
