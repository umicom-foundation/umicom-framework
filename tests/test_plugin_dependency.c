/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_plugin_dependency.c
 *
 * PURPOSE:
 *   Verify deterministic dependency ordering, missing dependencies and cycles.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "plugin_fixture.h"

int main(void)
{
    UmiPluginDependencyGraph *graph = NULL;
    UmiPluginDependency dependency;
    UmiPluginDependencyReport report;
    char id[UMI_PLUGIN_ID_CAPACITY];

    assert(umi_plugin_dependency_graph_create(&graph) == UMI_STATUS_OK);
    assert(umi_plugin_dependency_graph_add_plugin(
        graph, "org.umicom.base", plugin_test_version(1U, 0U, 0U)) == UMI_STATUS_OK);
    assert(umi_plugin_dependency_graph_add_plugin(
        graph, "org.umicom.editor", plugin_test_version(2U, 0U, 0U)) == UMI_STATUS_OK);
    (void)memset(&dependency, 0, sizeof(dependency));
    (void)strcpy(dependency.plugin_id, "org.umicom.editor");
    (void)strcpy(dependency.dependency_id, "org.umicom.base");
    dependency.minimum_version = plugin_test_version(1U, 0U, 0U);
    assert(umi_plugin_dependency_graph_add(graph, &dependency) == UMI_STATUS_OK);
    assert(umi_plugin_dependency_graph_resolve(graph, &report) == UMI_STATUS_OK);
    assert(report.resolved_count == 1U);
    assert(report.cycle_detected == 0);
    assert(umi_plugin_dependency_graph_order_count(graph) == 2U);
    assert(umi_plugin_dependency_graph_order_at(graph, 0U, id, sizeof(id)) == UMI_STATUS_OK);
    assert(strcmp(id, "org.umicom.base") == 0);
    umi_plugin_dependency_graph_destroy(graph);

    assert(umi_plugin_dependency_graph_create(&graph) == UMI_STATUS_OK);
    assert(umi_plugin_dependency_graph_add_plugin(
        graph, "one", plugin_test_version(1U, 0U, 0U)) == UMI_STATUS_OK);
    assert(umi_plugin_dependency_graph_add_plugin(
        graph, "two", plugin_test_version(1U, 0U, 0U)) == UMI_STATUS_OK);
    (void)memset(&dependency, 0, sizeof(dependency));
    (void)strcpy(dependency.plugin_id, "one");
    (void)strcpy(dependency.dependency_id, "two");
    assert(umi_plugin_dependency_graph_add(graph, &dependency) == UMI_STATUS_OK);
    (void)strcpy(dependency.plugin_id, "two");
    (void)strcpy(dependency.dependency_id, "one");
    assert(umi_plugin_dependency_graph_add(graph, &dependency) == UMI_STATUS_OK);
    assert(umi_plugin_dependency_graph_resolve(graph, &report) == UMI_STATUS_INVALID_STATE);
    assert(report.cycle_detected == 1);
    umi_plugin_dependency_graph_destroy(graph);
    return 0;
}
