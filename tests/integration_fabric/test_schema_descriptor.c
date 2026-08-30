/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_schema_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema descriptor Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/schema_descriptor.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricSchemaDescriptor item;
    CHECK(umi_fabric_schema_descriptor_init(&item,"trade","Trade",(UmiFabricVersion){1U,0U,0U},123U,true,false)==UMI_STATUS_OK);
    CHECK(item.backward_compatible);
    return 0;
}
