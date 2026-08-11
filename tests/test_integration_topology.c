/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_topology.c
 *
 * PURPOSE:
 *   Exercise one Suite and Inter-Application Runtime Foundation behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/topology.h"

#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s\n", #expr); return 1; } } while (0)

int main(void) {
    UmiIntegrationTopology t;
    UmiIntegrationDependency d={UMI_INTEGRATION_TARGET_CAPABILITY,UMI_INTEGRATION_DEPENDENCY_REQUIRED,"cap/1"};
    umi_integration_topology_init(&t);
    CHECK(umi_integration_topology_add(&t,"studio",&d)==UMI_STATUS_OK);
    CHECK(umi_integration_topology_required_edges(&t)==1U);
    return 0;
}
