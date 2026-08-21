/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout/test_traversal_query.c
 *
 * PURPOSE:
 *   Verify deterministic traversal, ancestry and semantic component queries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static bool count_visitor(
    void *context,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNode *node,
    size_t node_index,
    size_t depth)
{
    size_t *count = (size_t *)context;
    (void)document;
    (void)node;
    (void)node_index;
    (void)depth;
    *count += 1U;
    return true;
}

int main(void)
{
    UmiWorkbenchLayoutDocument *document =
        test_allocate_layout("layout.traversal");
    UmiWorkbenchLayoutTraversalResult traversal;
    UmiWorkbenchLayoutNodeQuery query;
    UmiWorkbenchLayoutNodeQueryResult result;
    size_t descendants[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t descendant_count = 0U;
    size_t visited = 0U;

    TEST_REQUIRE(document != NULL, "Fixture layout must be allocated");
    TEST_STATUS_OK(umi_workbench_layout_traverse(
        document,
        UMI_WORKBENCH_LAYOUT_TRAVERSAL_PRE_ORDER,
        count_visitor,
        &visited,
        &traversal));
    TEST_REQUIRE(visited == document->node_count, "Traversal must visit every node");
    TEST_REQUIRE(traversal.maximum_depth >= 2U, "Nested panels must report depth");

    TEST_STATUS_OK(umi_workbench_layout_collect_descendants(
        document,
        "centre",
        descendants,
        UMI_WORKBENCH_LAYOUT_MAX_NODES,
        &descendant_count));
    TEST_REQUIRE(descendant_count == 2U, "Centre split must own editor and output");
    TEST_REQUIRE(
        umi_workbench_layout_is_ancestor(document, "root", "chat"),
        "Root must be an ancestor of the chat panel");
    TEST_REQUIRE(
        umi_workbench_layout_node_depth(document, "output") == 2U,
        "Output panel depth must be deterministic");

    query = umi_workbench_layout_node_query_default();
    test_copy_text(
        query.owner_application_id,
        sizeof(query.owner_application_id),
        "org.umicom.studio");
    query.include_containers = false;
    query.include_leaf_nodes = true;
    TEST_STATUS_OK(umi_workbench_layout_query_nodes(
        document, &query, &result));
    TEST_REQUIRE(result.count == 4U, "Four Studio leaf panels are expected");

    query = umi_workbench_layout_node_query_default();
    test_copy_text(
        query.component_id,
        sizeof(query.component_id),
        "umicom.studio.editor");
    TEST_STATUS_OK(umi_workbench_layout_query_nodes(
        document, &query, &result));
    TEST_REQUIRE(result.count == 1U, "Editor component query must be unique");

    free(document);
    return 0;
}
