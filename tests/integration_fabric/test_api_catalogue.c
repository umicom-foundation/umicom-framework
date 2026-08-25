/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_api_catalogue.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the api catalogue Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/api_catalogue.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricApiCatalogue r; UmiFabricApiOperation item, found; umi_fabric_api_catalogue_init(&r);
    CHECK(umi_fabric_api_operation_init(&item,"get","GET","/x","none","x",true)==UMI_STATUS_OK);
    CHECK(umi_fabric_api_catalogue_add(&r,&item)==UMI_STATUS_OK);
    CHECK(umi_fabric_api_catalogue_add(&r,&item)==UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_fabric_api_catalogue_find(&r,item.operation_id,&found)==UMI_STATUS_OK);
    CHECK(umi_fabric_api_catalogue_count(&r)==1U);
    return 0;
}
