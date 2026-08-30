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

typedef struct UmiPluginExtensionHostDependencyCycle { uint8_t edges[UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY][UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY]; size_t node_count; } UmiPluginExtensionHostDependencyCycle;
void umi_plugin_extension_host_dependency_cycle_init(UmiPluginExtensionHostDependencyCycle *graph, size_t node_count);
UmiStatus umi_plugin_extension_host_dependency_cycle_add_edge(UmiPluginExtensionHostDependencyCycle *graph, size_t from, size_t to);
int umi_plugin_extension_host_dependency_cycle_has_cycle(const UmiPluginExtensionHostDependencyCycle *graph);

#ifdef __cplusplus
}
#endif

#endif
