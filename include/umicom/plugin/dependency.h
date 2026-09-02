/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/dependency.h
 *
 * PURPOSE:
 *   Describe plug-in dependencies and produce a deterministic activation order
 *   without loading executable extension code.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_DEPENDENCY_H
#define UMICOM_PLUGIN_DEPENDENCY_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/plugin/manifest.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLUGIN_DEPENDENCY_MAX 1024U
#define UMI_PLUGIN_DEPENDENCY_PLUGIN_MAX 512U

/**
 * Represent the plugin dependency data shared with callers of this public contract.
 */
typedef struct UmiPluginDependency {
    char plugin_id[UMI_PLUGIN_ID_CAPACITY];
    char dependency_id[UMI_PLUGIN_ID_CAPACITY];
    UmiVersion minimum_version;
    int optional;
} UmiPluginDependency;

/**
 * Represent the plugin dependency graph data shared with callers of this public contract.
 */
typedef struct UmiPluginDependencyGraph UmiPluginDependencyGraph;

/**
 * Represent the plugin dependency report data shared with callers of this public contract.
 */
typedef struct UmiPluginDependencyReport {
    size_t plugin_count;
    size_t dependency_count;
    size_t resolved_count;
    size_t missing_count;
    size_t incompatible_count;
    int cycle_detected;
    char first_problem[256];
} UmiPluginDependencyReport;

/**
 * Initialise plugin dependency graph from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_plugin_dependency_graph_create(UmiPluginDependencyGraph **out_graph);
/**
 * Release or reset state held by plugin dependency graph so the same storage can be reused
 * safely.
 */
void umi_plugin_dependency_graph_destroy(UmiPluginDependencyGraph *graph);
/**
 * Provide the plugin dependency graph add plugin operation used by this module and its
 * client applications.
 */
UmiStatus umi_plugin_dependency_graph_add_plugin(UmiPluginDependencyGraph *graph,
                                                 const char *plugin_id,
                                                 UmiVersion version);
/**
 * Add plugin dependency graph only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_plugin_dependency_graph_add(UmiPluginDependencyGraph *graph,
                                          const UmiPluginDependency *dependency);
/**
 * Provide the plugin dependency graph resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_dependency_graph_resolve(UmiPluginDependencyGraph *graph,
                                              UmiPluginDependencyReport *out_report);
/**
 * Return the number of records represented by plugin dependency graph order without
 * changing their state.
 */
size_t umi_plugin_dependency_graph_order_count(const UmiPluginDependencyGraph *graph);
/**
 * Find plugin dependency graph order while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_plugin_dependency_graph_order_at(const UmiPluginDependencyGraph *graph,
                                               size_t index,
                                               char *out_plugin_id,
                                               size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
