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

typedef struct UmiPluginDependency {
    char plugin_id[UMI_PLUGIN_ID_CAPACITY];
    char dependency_id[UMI_PLUGIN_ID_CAPACITY];
    UmiVersion minimum_version;
    int optional;
} UmiPluginDependency;

typedef struct UmiPluginDependencyGraph UmiPluginDependencyGraph;

typedef struct UmiPluginDependencyReport {
    size_t plugin_count;
    size_t dependency_count;
    size_t resolved_count;
    size_t missing_count;
    size_t incompatible_count;
    int cycle_detected;
    char first_problem[256];
} UmiPluginDependencyReport;

UmiStatus umi_plugin_dependency_graph_create(UmiPluginDependencyGraph **out_graph);
void umi_plugin_dependency_graph_destroy(UmiPluginDependencyGraph *graph);
UmiStatus umi_plugin_dependency_graph_add_plugin(UmiPluginDependencyGraph *graph,
                                                 const char *plugin_id,
                                                 UmiVersion version);
UmiStatus umi_plugin_dependency_graph_add(UmiPluginDependencyGraph *graph,
                                          const UmiPluginDependency *dependency);
UmiStatus umi_plugin_dependency_graph_resolve(UmiPluginDependencyGraph *graph,
                                              UmiPluginDependencyReport *out_report);
size_t umi_plugin_dependency_graph_order_count(const UmiPluginDependencyGraph *graph);
UmiStatus umi_plugin_dependency_graph_order_at(const UmiPluginDependencyGraph *graph,
                                               size_t index,
                                               char *out_plugin_id,
                                               size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
