/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_schema_evolution.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema evolution Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/schema_evolution.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricSchemaEvolution g; umi_fabric_schema_evolution_init(&g); CHECK(umi_fabric_schema_evolution_add(&g,"v1","v2",false)==UMI_STATUS_OK); CHECK(umi_fabric_schema_evolution_add(&g,"v2","v3",false)==UMI_STATUS_OK); CHECK(!umi_fabric_schema_evolution_has_cycle(&g)); CHECK(umi_fabric_schema_evolution_add(&g,"v3","v1",true)==UMI_STATUS_INVALID_STATE);
    return 0;
}
