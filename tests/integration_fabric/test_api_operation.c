/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_api_operation.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the api operation Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/api_operation.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricApiOperation item;
    CHECK(umi_fabric_api_operation_init(&item,"get.order","GET","/orders/{id}","none","order",true)==UMI_STATUS_OK);
    CHECK(item.idempotent);
    return 0;
}
