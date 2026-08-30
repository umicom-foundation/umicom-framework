/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_connector_binding.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the connector binding Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/connector_binding.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricConnectorBinding item;
    CHECK(umi_fabric_connector_binding_init(&item,"b","ibkr","ep","secure")==UMI_STATUS_OK);
    CHECK(item.enabled);
    return 0;
}
