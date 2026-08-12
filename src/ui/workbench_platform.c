/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workbench_platform.c
 *
 * PURPOSE:
 *   Implement the reusable high-level workbench platform and its owned toolkit-neutral models.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation owns only its documented child models. Product-specific
 * business data and toolkit objects remain outside this reusable Framework layer.
 */
#include "umicom/ui/workbench_platform.h"
#include <stdlib.h>
#include <string.h>

struct UmiUiWorkbenchPlatform {
    UmiUiListModelRegistry *lists; UmiUiTreeModelRegistry *trees; UmiUiSelectionModelRegistry *selection;
    UmiUiDockRegistry *docks; UmiUiTabRegistry *tabs; UmiUiPanelRegistry *panels;
    UmiUiContextMenuItemRegistry *context_menus; UmiUiInspectorPropertyRegistry *inspector;
    UmiUiCommandSurfaceRegistry *commands; UmiUiUndoStack *undo; uint64_t revision;
};

UmiStatus umi_ui_workbench_platform_create(UmiUiWorkbenchPlatform **out_platform)
{
    UmiUiWorkbenchPlatform *p; UmiStatus s = UMI_STATUS_OK;
    if (out_platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_platform = NULL;
    p = (UmiUiWorkbenchPlatform *)calloc(1U, sizeof(*p));
    if (p == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    p->revision = 1U;
    if (s == UMI_STATUS_OK) s = umi_ui_list_model_registry_create(&p->lists);
    if (s == UMI_STATUS_OK) s = umi_ui_tree_model_registry_create(&p->trees);
    if (s == UMI_STATUS_OK) s = umi_ui_selection_model_registry_create(&p->selection);
    if (s == UMI_STATUS_OK) s = umi_ui_dock_model_registry_create(&p->docks);
    if (s == UMI_STATUS_OK) s = umi_ui_tab_model_registry_create(&p->tabs);
    if (s == UMI_STATUS_OK) s = umi_ui_panel_model_registry_create(&p->panels);
    if (s == UMI_STATUS_OK) s = umi_ui_context_menu_registry_create(&p->context_menus);
    if (s == UMI_STATUS_OK) s = umi_ui_property_inspector_registry_create(&p->inspector);
    if (s == UMI_STATUS_OK) s = umi_ui_command_surface_registry_create(&p->commands);
    if (s == UMI_STATUS_OK) s = umi_ui_undo_stack_create(&p->undo);
    if (s != UMI_STATUS_OK) {
        umi_ui_workbench_platform_destroy(p);
        return s;
    }
    *out_platform = p;
    return UMI_STATUS_OK;
}

void umi_ui_workbench_platform_destroy(UmiUiWorkbenchPlatform *p)
{
    if (p == NULL) return;
    umi_ui_undo_stack_destroy(p->undo);
    umi_ui_command_surface_registry_destroy(p->commands);
    umi_ui_property_inspector_registry_destroy(p->inspector);
    umi_ui_context_menu_registry_destroy(p->context_menus);
    umi_ui_panel_model_registry_destroy(p->panels);
    umi_ui_tab_model_registry_destroy(p->tabs);
    umi_ui_dock_model_registry_destroy(p->docks);
    umi_ui_selection_model_registry_destroy(p->selection);
    umi_ui_tree_model_registry_destroy(p->trees);
    umi_ui_list_model_registry_destroy(p->lists);
    free(p);
}

UmiStatus umi_ui_workbench_platform_snapshot(const UmiUiWorkbenchPlatform *p, UmiUiWorkbenchPlatformSnapshot *o)
{
    if (p == NULL || o == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(o, 0, sizeof(*o));
    o->struct_size = (uint32_t)sizeof(*o);
    o->api_version = 1U;
    o->list_items=umi_ui_list_model_registry_count(p->lists); o->tree_items=umi_ui_tree_model_registry_count(p->trees);
    o->selections=umi_ui_selection_model_registry_count(p->selection); o->docks=umi_ui_dock_model_registry_count(p->docks);
    o->tabs=umi_ui_tab_model_registry_count(p->tabs); o->panels=umi_ui_panel_model_registry_count(p->panels);
    o->context_items=umi_ui_context_menu_registry_count(p->context_menus); o->inspector_items=umi_ui_property_inspector_registry_count(p->inspector);
    o->command_items=umi_ui_command_surface_registry_count(p->commands); o->undo_items=umi_ui_undo_stack_undo_count(p->undo);
    o->redo_items=umi_ui_undo_stack_redo_count(p->undo); o->revision=p->revision; return UMI_STATUS_OK;
}
#define ACCESSOR(fn,type,field) type *fn(UmiUiWorkbenchPlatform *p){return p!=NULL?p->field:NULL;}
ACCESSOR(umi_ui_workbench_platform_lists,UmiUiListModelRegistry,lists)
ACCESSOR(umi_ui_workbench_platform_trees,UmiUiTreeModelRegistry,trees)
ACCESSOR(umi_ui_workbench_platform_selection,UmiUiSelectionModelRegistry,selection)
ACCESSOR(umi_ui_workbench_platform_docks,UmiUiDockRegistry,docks)
ACCESSOR(umi_ui_workbench_platform_tabs,UmiUiTabRegistry,tabs)
ACCESSOR(umi_ui_workbench_platform_panels,UmiUiPanelRegistry,panels)
ACCESSOR(umi_ui_workbench_platform_context_menus,UmiUiContextMenuItemRegistry,context_menus)
ACCESSOR(umi_ui_workbench_platform_inspector,UmiUiInspectorPropertyRegistry,inspector)
ACCESSOR(umi_ui_workbench_platform_commands,UmiUiCommandSurfaceRegistry,commands)
ACCESSOR(umi_ui_workbench_platform_undo,UmiUiUndoStack,undo)
#undef ACCESSOR
