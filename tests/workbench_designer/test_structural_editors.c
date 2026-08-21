/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_structural_editors.c
 *
 * PURPOSE:
 *   Verify structural split, tab-group and floating-window editors produce
 *   stable semantic containers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchLayoutNode new_node;
    char container_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerRect floating_bounds = {420.0, 180.0, 520.0, 340.0};
    const UmiWorkbenchLayoutNode *container;

    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_layout_node_init(
        &new_node, "search", UMI_WORKBENCH_LAYOUT_NODE_PANEL);
    TEST_REQUIRE_STATUS(umi_workbench_layout_node_set_title(
        &new_node, "Search"));
    TEST_REQUIRE_STATUS(umi_workbench_layout_node_set_component(
        &new_node, "studio.search", "org.umicom.studio"));
    TEST_REQUIRE_STATUS(umi_workbench_designer_split_wrap_node(
        &document, "editor", &new_node,
        UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL, 0.72, true,
        container_id, sizeof(container_id)));
    container = umi_workbench_layout_document_find_node(
        &document, container_id);
    TEST_REQUIRE(container != NULL);
    TEST_REQUIRE(container->kind == UMI_WORKBENCH_LAYOUT_NODE_SPLIT);
    TEST_REQUIRE_STATUS(umi_workbench_designer_split_set_ratio(
        &document, container_id, 0.65));

    umi_workbench_layout_node_init(
        &new_node, "problems", UMI_WORKBENCH_LAYOUT_NODE_PANEL);
    TEST_REQUIRE_STATUS(umi_workbench_layout_node_set_title(
        &new_node, "Problems"));
    TEST_REQUIRE_STATUS(umi_workbench_layout_node_set_component(
        &new_node, "studio.problems", "org.umicom.studio"));
    TEST_REQUIRE_STATUS(umi_workbench_designer_tab_wrap_node(
        &document, "terminal", &new_node, true,
        container_id, sizeof(container_id)));
    TEST_REQUIRE_STATUS(umi_workbench_designer_tab_activate(
        &document, container_id, "problems"));
    TEST_REQUIRE_STATUS(umi_workbench_designer_float_node(
        &document, "project", "monitor.primary", floating_bounds, 10));
    TEST_REQUIRE(umi_workbench_layout_document_find_node(
        &document, "project") != NULL);
    return 0;
}
