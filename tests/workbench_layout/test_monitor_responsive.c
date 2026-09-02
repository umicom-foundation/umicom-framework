/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout/test_monitor_responsive.c
 *
 * PURPOSE:
 *   Verify monitor-independent placement and responsive layout policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

/*
 * Exercise make monitor and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiWorkbenchMonitor make_monitor(
    const char *monitor_id,
    int32_t x,
    int32_t width,
    bool primary)
{
    UmiWorkbenchMonitor monitor;
    (void)memset(&monitor, 0, sizeof(monitor));
    monitor.structure_size = sizeof(monitor);
    test_copy_text(
        monitor.monitor_id,
        sizeof(monitor.monitor_id),
        monitor_id);
    test_copy_text(
        monitor.display_name,
        sizeof(monitor.display_name),
        monitor_id);
    monitor.bounds.x = x;
    monitor.bounds.y = 0;
    monitor.bounds.width = width;
    monitor.bounds.height = 1080;
    monitor.work_area = monitor.bounds;
    monitor.work_area.height = 1040;
    monitor.scale = 1.0;
    monitor.refresh_rate_hz = 60.0;
    monitor.primary = primary;
    monitor.enabled = true;
    monitor.revision = 1U;
    return monitor;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchLayoutDocument *document =
        test_allocate_layout("layout.monitors");
    UmiWorkbenchMonitorTopology topology;
    UmiWorkbenchMonitor primary = make_monitor("primary", 0, 1920, true);
    UmiWorkbenchMonitor secondary = make_monitor("secondary", 1920, 1280, false);
    UmiWorkbenchLayoutNode workspace_window;
    UmiWorkbenchLayoutNode floating;
    UmiWorkbenchLayoutRect bounds = {5000, 200, 1400, 900};
    UmiWorkbenchLayoutPlacementReport placement;
    UmiWorkbenchResponsiveProfile responsive;
    UmiWorkbenchResponsiveRule rule;
    UmiWorkbenchResponsiveReport responsive_report;

    TEST_REQUIRE(document != NULL, "Fixture layout must be allocated");
    umi_workbench_monitor_topology_init(&topology);
    TEST_STATUS_OK(umi_workbench_monitor_topology_add(&topology, &primary));
    TEST_STATUS_OK(umi_workbench_monitor_topology_add(&topology, &secondary));
    TEST_REQUIRE(
        strcmp(
            umi_workbench_monitor_topology_primary(&topology)->monitor_id,
            "primary") == 0,
        "Primary monitor must be resolved explicitly");

    test_initialise_node(
        &workspace_window,
        "workspace.window",
        "Workbench Window",
        UMI_WORKBENCH_LAYOUT_NODE_WINDOW);
    TEST_STATUS_OK(umi_workbench_layout_document_add_node(
        document, &workspace_window, NULL));
    TEST_STATUS_OK(umi_workbench_layout_document_attach_child(
        document, "workspace.window", "root", 0U));

    test_initialise_node(
        &floating,
        "floating.database",
        "Database Explorer",
        UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW);
    TEST_STATUS_OK(umi_workbench_layout_node_set_bounds(&floating, &bounds));
    TEST_STATUS_OK(umi_workbench_layout_node_set_monitor(
        &floating, "removed-monitor"));
    TEST_STATUS_OK(umi_workbench_layout_document_add_node(
        document, &floating, NULL));
    TEST_STATUS_OK(umi_workbench_layout_document_attach_child(
        document,
        "workspace.window",
        "floating.database",
        UMI_WORKBENCH_LAYOUT_INDEX_NONE));
    TEST_STATUS_OK(umi_workbench_layout_place_document(
        document, &topology, NULL, &placement));
    TEST_REQUIRE(placement.changed, "Off-screen window must be relocated");
    TEST_REQUIRE(
        placement.missing_monitor_count == 1U,
        "Missing monitor must be recorded in placement evidence");
    TEST_REQUIRE(
        strcmp(
            umi_workbench_layout_document_find_node(
                document, "floating.database")->monitor_id,
            "primary") == 0,
        "Relocated window must use a valid monitor identity");

    umi_workbench_responsive_profile_init(
        &responsive, "compact", "Compact Workbench");
    (void)memset(&rule, 0, sizeof(rule));
    rule.structure_size = sizeof(rule);
    test_copy_text(rule.rule_id, sizeof(rule.rule_id), "hide-chat");
    test_copy_text(rule.node_id, sizeof(rule.node_id), "chat");
    rule.maximum_width = 1000;
    rule.maximum_height = INT32_MAX;
    rule.action = UMI_WORKBENCH_RESPONSIVE_HIDE_NODE;
    rule.priority = 10;
    rule.enabled = true;
    TEST_STATUS_OK(umi_workbench_responsive_profile_add_rule(
        &responsive, &rule));
    TEST_STATUS_OK(umi_workbench_responsive_apply(
        &responsive, 900, 700, document, &responsive_report));
    TEST_REQUIRE(
        responsive_report.applied_rule_count == 1U,
        "Compact profile must apply the matching rule");
    TEST_REQUIRE(
        umi_workbench_layout_document_find_node(
            document, "chat")->visibility ==
            UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN,
        "Chat panel must be hidden for compact workspaces");

    free(document);
    return 0;
}
