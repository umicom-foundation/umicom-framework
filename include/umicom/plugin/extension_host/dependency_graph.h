/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/dependency_graph.h
 *
 * PURPOSE:
 *   Maintain bounded extension dependency edges and dependency queries.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_DEPENDENCY_GRAPH_H
#define UMICOM_PLUGIN_EXTENSION_HOST_DEPENDENCY_GRAPH_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host dependency graph edge data shared with callers of
 * this public contract.
 */
typedef struct UmiPluginExtensionHostDependencyGraphEdge { char from[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; char to[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; int optional; } UmiPluginExtensionHostDependencyGraphEdge;
/**
 * Represent the plugin extension host dependency graph data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostDependencyGraph { UmiPluginExtensionHostDependencyGraphEdge edges[UMI_PLUGIN_EXTENSION_HOST_LARGE_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostDependencyGraph;
/**
 * Initialise plugin extension host dependency graph from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_dependency_graph_init(UmiPluginExtensionHostDependencyGraph *graph);
/**
 * Add plugin extension host dependency graph only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_plugin_extension_host_dependency_graph_add(UmiPluginExtensionHostDependencyGraph *graph, const char *from, const char *to, int optional);
/**
 * Provide the plugin extension host dependency graph depends on operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_dependency_graph_depends_on(const UmiPluginExtensionHostDependencyGraph *graph, const char *from, const char *to);
/**
 * Provide the plugin extension host dependency graph out degree operation used by this
 * module and its client applications.
 */
size_t umi_plugin_extension_host_dependency_graph_out_degree(const UmiPluginExtensionHostDependencyGraph *graph, const char *from);

#ifdef __cplusplus
}
#endif

#endif
