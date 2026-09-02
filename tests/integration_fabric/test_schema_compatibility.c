/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_schema_compatibility.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema compatibility Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/schema_compatibility.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricSchemaDescriptor a,b; CHECK(umi_fabric_schema_descriptor_init(&a,"a","Trade",(UmiFabricVersion){1U,1U,0U},1U,true,false)==UMI_STATUS_OK); CHECK(umi_fabric_schema_descriptor_init(&b,"b","Trade",(UmiFabricVersion){1U,0U,0U},2U,true,false)==UMI_STATUS_OK); CHECK(umi_fabric_schema_compatibility_classify(&a,&b)==UMI_FABRIC_SCHEMA_BACKWARD);
    return 0;
}
