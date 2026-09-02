/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_connector_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the connector descriptor Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/connector_descriptor.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricConnectorDescriptor item;
    CHECK(umi_fabric_connector_descriptor_init(&item,"ibkr","IBKR",UMI_FABRIC_PROTOCOL_TCP,7U,false)==UMI_STATUS_OK);
    CHECK(strcmp(item.provider,"IBKR")==0);
    return 0;
}
