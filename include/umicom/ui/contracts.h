/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/contracts.h
 *
 * PURPOSE:
 *   Aggregate the complete product-neutral Umicom UI contract without exposing toolkit object types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This aggregate includes Activity Bar, view-container, context, keybinding,
 * quick-access, Explorer, breadcrumb and workbench-state contracts.
 */

#ifndef UMICOM_UI_CONTRACTS_H
#define UMICOM_UI_CONTRACTS_H

#include "umicom/ui/accessibility.h"
#include "umicom/ui/action.h"
#include "umicom/ui/activity.h"
#include "umicom/ui/adapter.h"
#include "umicom/ui/application_shell.h"
#include "umicom/ui/appearance.h"
#include "umicom/ui/appearance_persistence.h"
#include "umicom/ui/binding.h"
#include "umicom/ui/breadcrumb.h"
#include "umicom/ui/clipboard.h"
#include "umicom/ui/context.h"
#include "umicom/ui/contribution.h"
#include "umicom/ui/dialog.h"
#include "umicom/ui/document_view.h"
#include "umicom/ui/document_layout.h"
#include "umicom/ui/editor.h"
#include "umicom/ui/editor_theme.h"
#include "umicom/ui/event.h"
#include "umicom/ui/explorer.h"
#include "umicom/ui/keybinding.h"
#include "umicom/ui/layout.h"
#include "umicom/ui/menu.h"
#include "umicom/ui/notification.h"
#include "umicom/ui/pane.h"
#include "umicom/ui/perspective.h"
#include "umicom/ui/property.h"
#include "umicom/ui/quick_access.h"
#include "umicom/ui/selection.h"
#include "umicom/ui/status.h"
#include "umicom/ui/theme.h"
#include "umicom/ui/toolbar.h"
#include "umicom/ui/types.h"
#include "umicom/ui/value.h"
#include "umicom/ui/view_container.h"
#include "umicom/ui/view_factory.h"
#include "umicom/ui/view_presentation.h"
#include "umicom/ui/view_model.h"
#include "umicom/ui/workbench.h"
#include "umicom/ui/workbench_state.h"
#include "umicom/ui/workspace_profile.h"
#include "umicom/ui/workspace_profile_persistence.h"
#include "umicom/ui/list_model.h"
#include "umicom/ui/tree_model.h"
#include "umicom/ui/selection_model.h"
#include "umicom/ui/sort_filter_model.h"
#include "umicom/ui/dock_model.h"
#include "umicom/ui/tab_model.h"
#include "umicom/ui/panel_model.h"
#include "umicom/ui/context_menu.h"
#include "umicom/ui/welcome_view.h"
#include "umicom/ui/property_inspector.h"
#include "umicom/ui/drag_drop.h"
#include "umicom/ui/extension_point.h"
#include "umicom/ui/command_surface.h"
#include "umicom/ui/command_view.h"
#include "umicom/ui/undo_stack.h"
#include "umicom/ui/workbench_platform.h"
#include "umicom/ui/problem.h"
#include "umicom/ui/output_channel.h"
#include "umicom/ui/progress.h"
#include "umicom/ui/task_monitor.h"
#include "umicom/ui/notification_item.h"
#include "umicom/ui/status_item.h"
#include "umicom/ui/command_history.h"
#include "umicom/ui/navigation_stack.h"
#include "umicom/ui/view_state.h"
#include "umicom/ui/workbench_services.h"

#ifdef __cplusplus
extern "C" {
#endif

/* This aggregate header intentionally contains no additional declarations. */

#ifdef __cplusplus
}
#endif
#endif
