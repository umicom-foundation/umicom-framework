/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_endpoint_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the endpoint descriptor Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/endpoint_descriptor.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricEndpointDescriptor item;
    CHECK(umi_fabric_endpoint_descriptor_init(&item,"e1","https://risk",UMI_FABRIC_PROTOCOL_HTTP,true,10U)==UMI_STATUS_OK);
    CHECK(item.secure);
    return 0;
}
