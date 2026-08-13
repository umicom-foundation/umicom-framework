/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/dependency.c
 *
 * PURPOSE:
 *   Resolve bounded plug-in dependency graphs using deterministic topological
 *   ordering and explicit missing, version and cycle diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/plugin/dependency.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct UmiPluginDependencyNode {
    char plugin_id[UMI_PLUGIN_ID_CAPACITY];
    UmiVersion version;
} UmiPluginDependencyNode;

struct UmiPluginDependencyGraph {
    UmiPluginDependencyNode nodes[UMI_PLUGIN_DEPENDENCY_PLUGIN_MAX];
    UmiPluginDependency dependencies[UMI_PLUGIN_DEPENDENCY_MAX];
    char order[UMI_PLUGIN_DEPENDENCY_PLUGIN_MAX][UMI_PLUGIN_ID_CAPACITY];
    size_t node_count;
    size_t dependency_count;
    size_t order_count;
};

static size_t node_find(const UmiPluginDependencyGraph *graph, const char *plugin_id)
{
    size_t index;
    for (index = 0U; index < graph->node_count; ++index) {
        if (strcmp(graph->nodes[index].plugin_id, plugin_id) == 0) return index;
    }
    return SIZE_MAX;
}

static int version_at_least(UmiVersion actual, UmiVersion minimum)
{
    if (actual.major != minimum.major) return actual.major > minimum.major;
    if (actual.minor != minimum.minor) return actual.minor > minimum.minor;
    return actual.patch >= minimum.patch;
}

UmiStatus umi_plugin_dependency_graph_create(UmiPluginDependencyGraph **out_graph)
{
    if (out_graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_graph = (UmiPluginDependencyGraph *)calloc(1U, sizeof(**out_graph));
    return *out_graph != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

void umi_plugin_dependency_graph_destroy(UmiPluginDependencyGraph *graph)
{
    free(graph);
}

UmiStatus umi_plugin_dependency_graph_add_plugin(UmiPluginDependencyGraph *graph,
                                                 const char *plugin_id,
                                                 UmiVersion version)
{
    UmiPluginDependencyNode *node;
    if (graph == NULL || plugin_id == NULL || plugin_id[0] == '\0' ||
        strlen(plugin_id) >= UMI_PLUGIN_ID_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT;
    if (node_find(graph, plugin_id) != SIZE_MAX) return UMI_STATUS_ALREADY_EXISTS;
    if (graph->node_count >= UMI_PLUGIN_DEPENDENCY_PLUGIN_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    node = &graph->nodes[graph->node_count++];
    (void)snprintf(node->plugin_id, sizeof(node->plugin_id), "%s", plugin_id);
    node->version = version;
    graph->order_count = 0U;
    return UMI_STATUS_OK;
}

UmiStatus umi_plugin_dependency_graph_add(UmiPluginDependencyGraph *graph,
                                          const UmiPluginDependency *dependency)
{
    size_t index;
    if (graph == NULL || dependency == NULL || dependency->plugin_id[0] == '\0' ||
        dependency->dependency_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < graph->dependency_count; ++index) {
        const UmiPluginDependency *item = &graph->dependencies[index];
        if (strcmp(item->plugin_id, dependency->plugin_id) == 0 &&
            strcmp(item->dependency_id, dependency->dependency_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (graph->dependency_count >= UMI_PLUGIN_DEPENDENCY_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    graph->dependencies[graph->dependency_count++] = *dependency;
    graph->order_count = 0U;
    return UMI_STATUS_OK;
}

UmiStatus umi_plugin_dependency_graph_resolve(UmiPluginDependencyGraph *graph,
                                              UmiPluginDependencyReport *out_report)
{
    size_t indegree[UMI_PLUGIN_DEPENDENCY_PLUGIN_MAX];
    int emitted[UMI_PLUGIN_DEPENDENCY_PLUGIN_MAX];
    size_t index;
    UmiStatus result = UMI_STATUS_OK;
    if (graph == NULL || out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    (void)memset(indegree, 0, sizeof(indegree));
    (void)memset(emitted, 0, sizeof(emitted));
    graph->order_count = 0U;
    out_report->plugin_count = graph->node_count;
    out_report->dependency_count = graph->dependency_count;

    for (index = 0U; index < graph->dependency_count; ++index) {
        const UmiPluginDependency *dependency = &graph->dependencies[index];
        size_t owner = node_find(graph, dependency->plugin_id);
        size_t target = node_find(graph, dependency->dependency_id);
        if (owner == SIZE_MAX) {
            ++out_report->missing_count;
            if (out_report->first_problem[0] == '\0') {
                (void)snprintf(out_report->first_problem,
                               sizeof(out_report->first_problem),
                                   "dependency owner is not registered: %.96s",
                               dependency->plugin_id);
            }
            result = UMI_STATUS_NOT_FOUND;
        } else if (target == SIZE_MAX) {
            if (!dependency->optional) {
                ++out_report->missing_count;
                if (out_report->first_problem[0] == '\0') {
                    (void)snprintf(out_report->first_problem,
                                   sizeof(out_report->first_problem),
                                   "%.96s requires missing plug-in %.96s",
                                   dependency->plugin_id,
                                   dependency->dependency_id);
                }
                result = UMI_STATUS_NOT_FOUND;
            }
        } else if (!version_at_least(graph->nodes[target].version,
                                     dependency->minimum_version)) {
            if (!dependency->optional) {
                ++out_report->incompatible_count;
                if (out_report->first_problem[0] == '\0') {
                    (void)snprintf(out_report->first_problem,
                                   sizeof(out_report->first_problem),
                                   "%.96s requires a newer %.96s",
                                   dependency->plugin_id,
                                   dependency->dependency_id);
                }
                result = UMI_STATUS_INVALID_STATE;
            }
        } else {
            ++indegree[owner];
            ++out_report->resolved_count;
        }
    }
    if (result != UMI_STATUS_OK) return result;

    while (graph->order_count < graph->node_count) {
        size_t selected = SIZE_MAX;
        for (index = 0U; index < graph->node_count; ++index) {
            if (!emitted[index] && indegree[index] == 0U &&
                (selected == SIZE_MAX ||
                 strcmp(graph->nodes[index].plugin_id,
                        graph->nodes[selected].plugin_id) < 0)) {
                selected = index;
            }
        }
        if (selected == SIZE_MAX) {
            out_report->cycle_detected = 1;
            (void)snprintf(out_report->first_problem,
                           sizeof(out_report->first_problem),
                           "dependency cycle prevents activation ordering");
            return UMI_STATUS_INVALID_STATE;
        }
        emitted[selected] = 1;
        (void)snprintf(graph->order[graph->order_count++],
                       UMI_PLUGIN_ID_CAPACITY,
                       "%s",
                       graph->nodes[selected].plugin_id);
        for (index = 0U; index < graph->dependency_count; ++index) {
            const UmiPluginDependency *dependency = &graph->dependencies[index];
            size_t owner;
            if (strcmp(dependency->dependency_id,
                       graph->nodes[selected].plugin_id) != 0) continue;
            owner = node_find(graph, dependency->plugin_id);
            if (owner != SIZE_MAX && indegree[owner] > 0U) --indegree[owner];
        }
    }
    (void)snprintf(out_report->first_problem,
                   sizeof(out_report->first_problem),
                   "all required dependencies resolved");
    return UMI_STATUS_OK;
}

size_t umi_plugin_dependency_graph_order_count(const UmiPluginDependencyGraph *graph)
{
    return graph != NULL ? graph->order_count : 0U;
}

UmiStatus umi_plugin_dependency_graph_order_at(const UmiPluginDependencyGraph *graph,
                                               size_t index,
                                               char *out_plugin_id,
                                               size_t capacity)
{
    size_t length;
    if (graph == NULL || out_plugin_id == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= graph->order_count) return UMI_STATUS_NOT_FOUND;
    length = strlen(graph->order[index]);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_plugin_id, graph->order[index], length + 1U);
    return UMI_STATUS_OK;
}
