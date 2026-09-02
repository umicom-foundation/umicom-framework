/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/os_service_graph.h
 *
 * PURPOSE:
 *   Build an acyclic Umicom OS service dependency graph and produce deterministic topological boot order.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_OS_SERVICE_GRAPH_H
#define UMICOM_PLATFORM_CROSS_TARGET_OS_SERVICE_GRAPH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/os_service_catalogue.h"
/**
 * Represent the ct os service graph data shared with callers of this public contract.
 */
typedef struct UmiCtOsServiceGraph { UmiCtOsServiceCatalogue catalogue; bool edges[UMI_CT_MAX_ITEMS][UMI_CT_MAX_ITEMS]; } UmiCtOsServiceGraph;
/**
 * Initialise ct os service graph from caller-provided values so later operations receive a
 * known state.
 */
void umi_ct_os_service_graph_init(UmiCtOsServiceGraph *graph);
/**
 * Add ct os service graph only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ct_os_service_graph_add(UmiCtOsServiceGraph *graph,const UmiCtOsServiceDescriptor *service);
/**
 * Provide the ct os service graph link operation used by this module and its client
 * applications.
 */
UmiStatus umi_ct_os_service_graph_link(UmiCtOsServiceGraph *graph,const char *service_id,const char *dependency_id);
/**
 * Provide the ct os service graph order operation used by this module and its client
 * applications.
 */
UmiStatus umi_ct_os_service_graph_order(const UmiCtOsServiceGraph *graph,size_t *out_indices,size_t capacity,size_t *out_count);

#ifdef __cplusplus
}
#endif

#endif
