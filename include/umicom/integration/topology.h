/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/topology.h
 *
 * PURPOSE:
 *   Represent the public application/capability dependency graph.
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

#ifndef UMICOM_INTEGRATION_TOPOLOGY_H
#define UMICOM_INTEGRATION_TOPOLOGY_H

#include "umicom/base/status.h"
#include "umicom/integration/dependency.h"

/**
 * Represent the integration topology edge data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationTopologyEdge {
    char source_application[UMI_INTEGRATION_ID_CAPACITY];
    UmiIntegrationDependency dependency;
} UmiIntegrationTopologyEdge;

/**
 * Represent the integration topology data shared with callers of this public contract.
 */
typedef struct UmiIntegrationTopology {
    UmiIntegrationTopologyEdge edges[UMI_INTEGRATION_MAX_EDGES];
    size_t count;
} UmiIntegrationTopology;

/**
 * Initialise integration topology from caller-provided values so later operations receive
 * a known state.
 */
void umi_integration_topology_init(UmiIntegrationTopology *topology);
/**
 * Add integration topology only after its inputs and available capacity have been checked.
 */
UmiStatus umi_integration_topology_add(
    UmiIntegrationTopology *topology,
    const char *source_application,
    const UmiIntegrationDependency *dependency);
/**
 * Provide the integration topology required edges operation used by this module and its
 * client applications.
 */
size_t umi_integration_topology_required_edges(
    const UmiIntegrationTopology *topology);

#endif
