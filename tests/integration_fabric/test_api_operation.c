/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_api_operation.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the api operation Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/api_operation.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricApiOperation item;
    CHECK(umi_fabric_api_operation_init(&item,"get.order","GET","/orders/{id}","none","order",true)==UMI_STATUS_OK);
    CHECK(item.idempotent);
    return 0;
}
