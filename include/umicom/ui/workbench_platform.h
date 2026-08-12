/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workbench_platform.h
 *
 * PURPOSE:
 *   Define a reusable high-level workbench platform that owns common list, tree, selection, dock, tab, panel, command, inspector and undo models.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract is toolkit-neutral and intended for reuse by Studio,
 * Trader, TMS and future Umicom applications without exposing GUI objects.
 */
#ifndef UMICOM_UI_WORKBENCH_PLATFORM_H
#define UMICOM_UI_WORKBENCH_PLATFORM_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/list_model.h"
#include "umicom/ui/tree_model.h"
#include "umicom/ui/selection_model.h"
#include "umicom/ui/dock_model.h"
#include "umicom/ui/tab_model.h"
#include "umicom/ui/panel_model.h"
#include "umicom/ui/context_menu.h"
#include "umicom/ui/property_inspector.h"
#include "umicom/ui/command_surface.h"
#include "umicom/ui/undo_stack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiWorkbenchPlatform UmiUiWorkbenchPlatform;
typedef struct UmiUiWorkbenchPlatformSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t list_items;
    size_t tree_items;
    size_t selections;
    size_t docks;
    size_t tabs;
    size_t panels;
    size_t context_items;
    size_t inspector_items;
    size_t command_items;
    size_t undo_items;
    size_t redo_items;
    uint64_t revision;
} UmiUiWorkbenchPlatformSnapshot;

UmiStatus umi_ui_workbench_platform_create(UmiUiWorkbenchPlatform **out_platform);
void umi_ui_workbench_platform_destroy(UmiUiWorkbenchPlatform *platform);
UmiStatus umi_ui_workbench_platform_snapshot(const UmiUiWorkbenchPlatform *platform, UmiUiWorkbenchPlatformSnapshot *out_snapshot);
UmiUiListModelRegistry *umi_ui_workbench_platform_lists(UmiUiWorkbenchPlatform *platform);
UmiUiTreeModelRegistry *umi_ui_workbench_platform_trees(UmiUiWorkbenchPlatform *platform);
UmiUiSelectionModelRegistry *umi_ui_workbench_platform_selection(UmiUiWorkbenchPlatform *platform);
UmiUiDockRegistry *umi_ui_workbench_platform_docks(UmiUiWorkbenchPlatform *platform);
UmiUiTabRegistry *umi_ui_workbench_platform_tabs(UmiUiWorkbenchPlatform *platform);
UmiUiPanelRegistry *umi_ui_workbench_platform_panels(UmiUiWorkbenchPlatform *platform);
UmiUiContextMenuItemRegistry *umi_ui_workbench_platform_context_menus(UmiUiWorkbenchPlatform *platform);
UmiUiInspectorPropertyRegistry *umi_ui_workbench_platform_inspector(UmiUiWorkbenchPlatform *platform);
UmiUiCommandSurfaceRegistry *umi_ui_workbench_platform_commands(UmiUiWorkbenchPlatform *platform);
UmiUiUndoStack *umi_ui_workbench_platform_undo(UmiUiWorkbenchPlatform *platform);

#ifdef __cplusplus
}
#endif
#endif
