/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/topology.h
 *
 * PURPOSE:
 *   Represent the public application/capability dependency graph.
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

#ifndef UMICOM_INTEGRATION_TOPOLOGY_H
#define UMICOM_INTEGRATION_TOPOLOGY_H

#include "umicom/base/status.h"
#include "umicom/integration/dependency.h"

typedef struct UmiIntegrationTopologyEdge {
    char source_application[UMI_INTEGRATION_ID_CAPACITY];
    UmiIntegrationDependency dependency;
} UmiIntegrationTopologyEdge;

typedef struct UmiIntegrationTopology {
    UmiIntegrationTopologyEdge edges[UMI_INTEGRATION_MAX_EDGES];
    size_t count;
} UmiIntegrationTopology;

void umi_integration_topology_init(UmiIntegrationTopology *topology);
UmiStatus umi_integration_topology_add(
    UmiIntegrationTopology *topology,
    const char *source_application,
    const UmiIntegrationDependency *dependency);
size_t umi_integration_topology_required_edges(
    const UmiIntegrationTopology *topology);

#endif
