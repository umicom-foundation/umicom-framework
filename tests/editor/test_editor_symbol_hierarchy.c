/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_symbol_hierarchy.c
 *
 * PURPOSE:
 *   Implement the test editor symbol hierarchy behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework reusable symbol hierarchy tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/symbol_hierarchy.h"

/*
 * Exercise node and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiEditorSymbolHierarchyNode node(const char *node_id,
                                         const char *parent_id,
                                         const char *symbol_id,
                                         size_t depth)
{
    UmiEditorSymbolHierarchyNode value = {0};
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION;
    (void)strcpy(value.node_id, node_id);
    (void)strcpy(value.parent_node_id, parent_id);
    (void)strcpy(value.symbol_id, symbol_id);
    (void)strcpy(value.provider_id, "test-provider");
    (void)strcpy(value.label, symbol_id);
    value.kind = parent_id[0] == '\0'
        ? UMI_EDITOR_SYMBOL_HIERARCHY_ROOT
        : UMI_EDITOR_SYMBOL_HIERARCHY_OUTGOING_CALL;
    value.symbol_kind = UMI_EDITOR_INDEXED_SYMBOL_FUNCTION;
    assert(umi_editor_source_location_initialize(&value.location,
                                                  "file:///tree.c",
                                                  (uint64_t)depth + 1U,
                                                  0U) == UMI_STATUS_OK);
    value.depth = depth;
    value.relationship_count = 1U;
    value.expanded = 1;
    value.selected = parent_id[0] == '\0';
    return value;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorSymbolHierarchy *hierarchy = NULL;
    UmiEditorSymbolHierarchyNode root = node("root", "", "main", 0U);
    UmiEditorSymbolHierarchyNode child = node("child", "root", "run", 1U);
    UmiEditorSymbolHierarchyNode leaf = node("leaf", "child", "work", 2U);
    UmiEditorSymbolHierarchyNode selected;
    UmiEditorSymbolHierarchySnapshot snapshot;
    UmiEditorSymbolHierarchyBuildOptions options;

    assert(umi_editor_symbol_hierarchy_create(&hierarchy) == UMI_STATUS_OK);
    assert(umi_editor_symbol_hierarchy_add(hierarchy, &root) == UMI_STATUS_OK);
    assert(umi_editor_symbol_hierarchy_add(hierarchy, &child) == UMI_STATUS_OK);
    assert(umi_editor_symbol_hierarchy_add(hierarchy, &leaf) == UMI_STATUS_OK);
    assert(umi_editor_symbol_hierarchy_finalize(hierarchy, 0) == UMI_STATUS_OK);
    assert(umi_editor_symbol_hierarchy_visible_count(hierarchy) == 3U);
    assert(umi_editor_symbol_hierarchy_select(hierarchy, "leaf") ==
           UMI_STATUS_OK);
    assert(umi_editor_symbol_hierarchy_set_expanded(hierarchy, "root", 0) ==
           UMI_STATUS_OK);
    assert(umi_editor_symbol_hierarchy_visible_count(hierarchy) == 1U);
    assert(umi_editor_symbol_hierarchy_selected(hierarchy, &selected) ==
           UMI_STATUS_OK);
    assert(strcmp(selected.node_id, "root") == 0);
    assert(umi_editor_symbol_hierarchy_snapshot(hierarchy, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.node_count == 3U && snapshot.visible_node_count == 1U);
    assert(umi_editor_symbol_hierarchy_build_options_initialize(&options) ==
           UMI_STATUS_OK);
    assert(options.maximum_depth == UMI_EDITOR_HIERARCHY_DEFAULT_MAXIMUM_DEPTH);
    umi_editor_symbol_hierarchy_destroy(hierarchy);
    return 0;
}
