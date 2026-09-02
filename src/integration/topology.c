/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/topology.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/topology.h"

#include <stdio.h>
#include <string.h>

/*
 * Initialise integration topology from caller-provided values so later operations receive
 * a known state.
 */
void umi_integration_topology_init(UmiIntegrationTopology *topology)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology != NULL) {
        (void)memset(topology, 0, sizeof(*topology));
    }
}

/* Add integration topology only after its inputs and available capacity have been checked. */
UmiStatus umi_integration_topology_add(
    UmiIntegrationTopology *topology,
    const char *source_application,
    const UmiIntegrationDependency *dependency)
{
    UmiIntegrationTopologyEdge *edge;
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL || source_application == NULL ||
        dependency == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (topology->count >= UMI_INTEGRATION_MAX_EDGES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    edge = &topology->edges[topology->count];
    written = snprintf(edge->source_application,
                       sizeof(edge->source_application),
                       "%s",
                       source_application);
    /* Apply this branch only when its contract condition is satisfied. */
    if (written < 0 ||
        (size_t)written >= sizeof(edge->source_application)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    edge->dependency = *dependency;
    ++topology->count;
    return UMI_STATUS_OK;
}

/*
 * Provide the integration topology required edges operation used by this module and its
 * client applications.
 */
size_t umi_integration_topology_required_edges(
    const UmiIntegrationTopology *topology)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topology == NULL) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < topology->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (topology->edges[index].dependency.kind ==
            UMI_INTEGRATION_DEPENDENCY_REQUIRED) {
            ++count;
        }
    }
    return count;
}
