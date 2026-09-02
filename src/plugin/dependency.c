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

/* Find node while leaving the underlying catalogue or model owned by this module. */
static size_t node_find(const UmiPluginDependencyGraph *graph, const char *plugin_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < graph->node_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(graph->nodes[index].plugin_id, plugin_id) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the version at least operation used by this module and its client applications. */
static int version_at_least(UmiVersion actual, UmiVersion minimum)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (actual.major != minimum.major) return actual.major > minimum.major;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (actual.minor != minimum.minor) return actual.minor > minimum.minor;
    return actual.patch >= minimum.patch;
}

/*
 * Initialise plugin dependency graph from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_plugin_dependency_graph_create(UmiPluginDependencyGraph **out_graph)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_graph = (UmiPluginDependencyGraph *)calloc(1U, sizeof(**out_graph));
    return *out_graph != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

/*
 * Release or reset state held by plugin dependency graph so the same storage can be reused
 * safely.
 */
void umi_plugin_dependency_graph_destroy(UmiPluginDependencyGraph *graph)
{
    free(graph);
}

/*
 * Provide the plugin dependency graph add plugin operation used by this module and its
 * client applications.
 */
UmiStatus umi_plugin_dependency_graph_add_plugin(UmiPluginDependencyGraph *graph,
                                                 const char *plugin_id,
                                                 UmiVersion version)
{
    UmiPluginDependencyNode *node;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || plugin_id == NULL || plugin_id[0] == '\0' ||
        strlen(plugin_id) >= UMI_PLUGIN_ID_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (node_find(graph, plugin_id) != SIZE_MAX) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (graph->node_count >= UMI_PLUGIN_DEPENDENCY_PLUGIN_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    node = &graph->nodes[graph->node_count++];
    (void)snprintf(node->plugin_id, sizeof(node->plugin_id), "%s", plugin_id);
    node->version = version;
    graph->order_count = 0U;
    return UMI_STATUS_OK;
}

/*
 * Add plugin dependency graph only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_plugin_dependency_graph_add(UmiPluginDependencyGraph *graph,
                                          const UmiPluginDependency *dependency)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || dependency == NULL || dependency->plugin_id[0] == '\0' ||
        dependency->dependency_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < graph->dependency_count; ++index) {
        const UmiPluginDependency *item = &graph->dependencies[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(item->plugin_id, dependency->plugin_id) == 0 &&
            strcmp(item->dependency_id, dependency->dependency_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (graph->dependency_count >= UMI_PLUGIN_DEPENDENCY_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    graph->dependencies[graph->dependency_count++] = *dependency;
    graph->order_count = 0U;
    return UMI_STATUS_OK;
}

/*
 * Provide the plugin dependency graph resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_dependency_graph_resolve(UmiPluginDependencyGraph *graph,
                                              UmiPluginDependencyReport *out_report)
{
    size_t indegree[UMI_PLUGIN_DEPENDENCY_PLUGIN_MAX];
    int emitted[UMI_PLUGIN_DEPENDENCY_PLUGIN_MAX];
    size_t index;
    UmiStatus result = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    (void)memset(indegree, 0, sizeof(indegree));
    (void)memset(emitted, 0, sizeof(emitted));
    graph->order_count = 0U;
    out_report->plugin_count = graph->node_count;
    out_report->dependency_count = graph->dependency_count;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < graph->dependency_count; ++index) {
        const UmiPluginDependency *dependency = &graph->dependencies[index];
        size_t owner = node_find(graph, dependency->plugin_id);
        size_t target = node_find(graph, dependency->dependency_id);
        /* Apply this branch only when its contract condition is satisfied. */
        if (owner == SIZE_MAX) {
            ++out_report->missing_count;
            /* Apply this branch only when its contract condition is satisfied. */
            if (out_report->first_problem[0] == '\0') {
                (void)snprintf(out_report->first_problem,
                               sizeof(out_report->first_problem),
                                   "dependency owner is not registered: %.96s",
                               dependency->plugin_id);
            }
            result = UMI_STATUS_NOT_FOUND;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (target == SIZE_MAX) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (!dependency->optional) {
                ++out_report->missing_count;
                /* Apply this branch only when its contract condition is satisfied. */
                if (out_report->first_problem[0] == '\0') {
                    (void)snprintf(out_report->first_problem,
                                   sizeof(out_report->first_problem),
                                   "%.96s requires missing plug-in %.96s",
                                   dependency->plugin_id,
                                   dependency->dependency_id);
                }
                result = UMI_STATUS_NOT_FOUND;
            }
        } else /* Apply this branch only when its contract condition is satisfied. */ if (!version_at_least(graph->nodes[target].version,
                                     dependency->minimum_version)) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (!dependency->optional) {
                ++out_report->incompatible_count;
                /* Apply this branch only when its contract condition is satisfied. */
                if (out_report->first_problem[0] == '\0') {
                    (void)snprintf(out_report->first_problem,
                                   sizeof(out_report->first_problem),
                                   "%.96s requires a newer %.96s",
                                   dependency->plugin_id,
                                   dependency->dependency_id);
                }
                result = UMI_STATUS_INVALID_STATE;
            }
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            ++indegree[owner];
            ++out_report->resolved_count;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (graph->order_count < graph->node_count) {
        size_t selected = SIZE_MAX;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < graph->node_count; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (!emitted[index] && indegree[index] == 0U &&
                (selected == SIZE_MAX ||
                 strcmp(graph->nodes[index].plugin_id,
                        graph->nodes[selected].plugin_id) < 0)) {
                selected = index;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
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
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < graph->dependency_count; ++index) {
            const UmiPluginDependency *dependency = &graph->dependencies[index];
            size_t owner;
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(dependency->dependency_id,
                       graph->nodes[selected].plugin_id) != 0) continue;
            owner = node_find(graph, dependency->plugin_id);
            /* Apply this branch only when its contract condition is satisfied. */
            if (owner != SIZE_MAX && indegree[owner] > 0U) --indegree[owner];
        }
    }
    (void)snprintf(out_report->first_problem,
                   sizeof(out_report->first_problem),
                   "all required dependencies resolved");
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by plugin dependency graph order without
 * changing their state.
 */
size_t umi_plugin_dependency_graph_order_count(const UmiPluginDependencyGraph *graph)
{
    return graph != NULL ? graph->order_count : 0U;
}

/*
 * Find plugin dependency graph order while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_plugin_dependency_graph_order_at(const UmiPluginDependencyGraph *graph,
                                               size_t index,
                                               char *out_plugin_id,
                                               size_t capacity)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || out_plugin_id == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= graph->order_count) return UMI_STATUS_NOT_FOUND;
    length = strlen(graph->order[index]);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_plugin_id, graph->order[index], length + 1U);
    return UMI_STATUS_OK;
}
