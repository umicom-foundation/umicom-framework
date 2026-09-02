/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workbench_platform.h
 *
 * PURPOSE:
 *   Define a reusable high-level workbench platform that owns common list, tree, selection, dock, tab, panel, command, inspector and undo models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the ui workbench platform data shared with callers of this public contract.
 */
typedef struct UmiUiWorkbenchPlatform UmiUiWorkbenchPlatform;
/**
 * Represent the ui workbench platform snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise ui workbench platform from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_workbench_platform_create(UmiUiWorkbenchPlatform **out_platform);
/**
 * Release or reset state held by ui workbench platform so the same storage can be reused
 * safely.
 */
void umi_ui_workbench_platform_destroy(UmiUiWorkbenchPlatform *platform);
/**
 * Provide the ui workbench platform snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_platform_snapshot(const UmiUiWorkbenchPlatform *platform, UmiUiWorkbenchPlatformSnapshot *out_snapshot);
/**
 * Provide the ui workbench platform lists operation used by this module and its client
 * applications.
 */
UmiUiListModelRegistry *umi_ui_workbench_platform_lists(UmiUiWorkbenchPlatform *platform);
/**
 * Provide the ui workbench platform trees operation used by this module and its client
 * applications.
 */
UmiUiTreeModelRegistry *umi_ui_workbench_platform_trees(UmiUiWorkbenchPlatform *platform);
/**
 * Provide the ui workbench platform selection operation used by this module and its client
 * applications.
 */
UmiUiSelectionModelRegistry *umi_ui_workbench_platform_selection(UmiUiWorkbenchPlatform *platform);
/**
 * Provide the ui workbench platform docks operation used by this module and its client
 * applications.
 */
UmiUiDockRegistry *umi_ui_workbench_platform_docks(UmiUiWorkbenchPlatform *platform);
/**
 * Provide the ui workbench platform tabs operation used by this module and its client
 * applications.
 */
UmiUiTabRegistry *umi_ui_workbench_platform_tabs(UmiUiWorkbenchPlatform *platform);
/**
 * Provide the ui workbench platform panels operation used by this module and its client
 * applications.
 */
UmiUiPanelRegistry *umi_ui_workbench_platform_panels(UmiUiWorkbenchPlatform *platform);
/**
 * Provide the ui workbench platform context menus operation used by this module and its
 * client applications.
 */
UmiUiContextMenuItemRegistry *umi_ui_workbench_platform_context_menus(UmiUiWorkbenchPlatform *platform);
/**
 * Provide the ui workbench platform inspector operation used by this module and its client
 * applications.
 */
UmiUiInspectorPropertyRegistry *umi_ui_workbench_platform_inspector(UmiUiWorkbenchPlatform *platform);
/**
 * Provide the ui workbench platform commands operation used by this module and its client
 * applications.
 */
UmiUiCommandSurfaceRegistry *umi_ui_workbench_platform_commands(UmiUiWorkbenchPlatform *platform);
/**
 * Provide the ui workbench platform undo operation used by this module and its client
 * applications.
 */
UmiUiUndoStack *umi_ui_workbench_platform_undo(UmiUiWorkbenchPlatform *platform);

#ifdef __cplusplus
}
#endif
#endif
