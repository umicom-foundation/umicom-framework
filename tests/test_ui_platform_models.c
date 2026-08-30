/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_platform_models.c
 *
 * PURPOSE:
 *   Exercise the Batch 25 reusable workbench list, tree, filtering, tab,
 *   panel, dock and undo primitives with deterministic smoke coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <string.h>

#include "umicom/ui/dock_model.h"
#include "umicom/ui/list_model.h"
#include "umicom/ui/panel_model.h"
#include "umicom/ui/sort_filter_model.h"
#include "umicom/ui/tab_model.h"
#include "umicom/ui/tree_model.h"
#include "umicom/ui/undo_stack.h"

int main(void)
{
    UmiUiListModelRegistry *list = NULL;
    UmiUiListModelSnapshot list_item = {0};
    UmiUiTreeModelRegistry *tree = NULL;
    UmiUiTreeModelSnapshot root = {0};
    UmiUiTreeModelSnapshot child = {0};
    UmiUiTreeModelSnapshot found_child = {0};
    UmiUiSortFilterSnapshot filter = {0};
    UmiUiTabRegistry *tabs = NULL;
    UmiUiTabSnapshot first_tab = {0};
    UmiUiTabSnapshot second_tab = {0};
    UmiUiTabSnapshot tab_result = {0};
    UmiUiPanelRegistry *panels = NULL;
    UmiUiPanelSnapshot panel = {0};
    UmiUiDockRegistry *docks = NULL;
    UmiUiDockSnapshot dock = {0};
    UmiUiUndoStack *undo = NULL;
    UmiUiUndoEntry edit = {0};
    UmiUiUndoEntry undone = {0};

    if (umi_ui_list_model_registry_create(&list) != UMI_STATUS_OK) return 1;
    (void)strcpy(list_item.id, "one");
    (void)strcpy(list_item.label, "One");
    list_item.visible = 1;
    list_item.enabled = 1;
    if (umi_ui_list_model_registry_upsert(list, &list_item) != UMI_STATUS_OK ||
        umi_ui_list_model_registry_count(list) != 1U) return 2;

    if (umi_ui_tree_model_registry_create(&tree) != UMI_STATUS_OK) return 3;
    (void)strcpy(root.id, "root");
    (void)strcpy(root.label, "Root");
    root.selectable = 1;
    if (umi_ui_tree_model_registry_upsert(tree, &root) != UMI_STATUS_OK) return 4;
    (void)strcpy(child.id, "child");
    (void)strcpy(child.parent_id, "root");
    (void)strcpy(child.label, "Child");
    child.selectable = 1;
    if (umi_ui_tree_model_registry_upsert(tree, &child) != UMI_STATUS_OK) return 5;
    if (umi_ui_tree_model_registry_set_expanded(tree, "root", 1) != UMI_STATUS_OK) return 6;
    if (umi_ui_tree_model_registry_child_count(tree, "root") != 1U) return 7;
    if (umi_ui_tree_model_registry_child_at(tree, "root", 0U, &found_child) != UMI_STATUS_OK ||
        strcmp(found_child.id, "child") != 0) return 8;

    filter.enabled = 1;
    filter.case_sensitive = 0;
    filter.ascending = 1;
    (void)strcpy(filter.query, "work");
    if (!umi_ui_sort_filter_model_matches(&filter, "Workspace")) return 9;
    if (umi_ui_sort_filter_model_matches(&filter, "Editor")) return 10;
    if (umi_ui_sort_filter_model_compare_text(&filter, "Alpha", "beta") >= 0) return 11;

    if (umi_ui_tab_model_registry_create(&tabs) != UMI_STATUS_OK) return 12;
    (void)strcpy(first_tab.id, "one.c");
    (void)strcpy(first_tab.group_id, "editor");
    first_tab.active = 1;
    first_tab.closable = 1;
    if (umi_ui_tab_model_registry_upsert(tabs, &first_tab) != UMI_STATUS_OK) return 13;
    (void)strcpy(second_tab.id, "two.c");
    (void)strcpy(second_tab.group_id, "editor");
    second_tab.closable = 1;
    if (umi_ui_tab_model_registry_upsert(tabs, &second_tab) != UMI_STATUS_OK) return 14;
    if (umi_ui_tab_model_registry_activate(tabs, "two.c") != UMI_STATUS_OK) return 15;
    if (umi_ui_tab_model_registry_set_dirty(tabs, "two.c", 1) != UMI_STATUS_OK) return 16;
    if (umi_ui_tab_model_registry_find(tabs, "two.c", &tab_result) != UMI_STATUS_OK ||
        !tab_result.active || !tab_result.dirty) return 17;

    if (umi_ui_panel_model_registry_create(&panels) != UMI_STATUS_OK) return 18;
    (void)strcpy(panel.id, "problems");
    panel.visible = 1;
    if (umi_ui_panel_model_registry_upsert(panels, &panel) != UMI_STATUS_OK) return 19;
    if (umi_ui_panel_model_registry_toggle_visible(panels, "problems") != UMI_STATUS_OK) return 20;
    if (umi_ui_panel_model_registry_find(panels, "problems", &panel) != UMI_STATUS_OK || panel.visible) return 21;

    if (umi_ui_dock_model_registry_create(&docks) != UMI_STATUS_OK) return 22;
    (void)strcpy(dock.id, "left");
    (void)strcpy(dock.area, "left");
    dock.visible = 1;
    if (umi_ui_dock_model_registry_upsert(docks, &dock) != UMI_STATUS_OK) return 23;

    if (umi_ui_undo_stack_create(&undo) != UMI_STATUS_OK) return 24;
    (void)strcpy(edit.id, "edit");
    if (umi_ui_undo_stack_push(undo, &edit) != UMI_STATUS_OK ||
        umi_ui_undo_stack_undo(undo, &undone) != UMI_STATUS_OK) return 25;

    umi_ui_undo_stack_destroy(undo);
    umi_ui_dock_model_registry_destroy(docks);
    umi_ui_panel_model_registry_destroy(panels);
    umi_ui_tab_model_registry_destroy(tabs);
    umi_ui_tree_model_registry_destroy(tree);
    umi_ui_list_model_registry_destroy(list);
    return 0;
}
