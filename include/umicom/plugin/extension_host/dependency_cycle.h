/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/dependency_cycle.h
 *
 * PURPOSE:
 *   Detect cycles in extension dependency topology before activation.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_DEPENDENCY_CYCLE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_DEPENDENCY_CYCLE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host dependency cycle data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostDependencyCycle { uint8_t edges[UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY][UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY]; size_t node_count; } UmiPluginExtensionHostDependencyCycle;
/**
 * Initialise plugin extension host dependency cycle from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_dependency_cycle_init(UmiPluginExtensionHostDependencyCycle *graph, size_t node_count);
/**
 * Provide the plugin extension host dependency cycle add edge operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_dependency_cycle_add_edge(UmiPluginExtensionHostDependencyCycle *graph, size_t from, size_t to);
/**
 * Provide the plugin extension host dependency cycle has cycle operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_dependency_cycle_has_cycle(const UmiPluginExtensionHostDependencyCycle *graph);

#ifdef __cplusplus
}
#endif

#endif
