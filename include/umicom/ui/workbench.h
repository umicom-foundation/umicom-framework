/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workbench.h
 *
 * PURPOSE:
 *   Define the authoritative toolkit-neutral workbench aggregate including activities, view containers, context, keybindings, quick access, Explorer and breadcrumbs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Batch 23 extends the existing workbench rather than creating a competing shell. Products compose these Framework models and frontend adapters render them.
 */

#ifndef UMICOM_UI_WORKBENCH_H
#define UMICOM_UI_WORKBENCH_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/runtime/command_registry.h"
#include "umicom/ui/action.h"
#include "umicom/ui/activity.h"
#include "umicom/ui/binding.h"
#include "umicom/ui/breadcrumb.h"
#include "umicom/ui/context.h"
#include "umicom/ui/contribution.h"
#include "umicom/ui/dialog.h"
#include "umicom/ui/document_view.h"
#include "umicom/ui/editor.h"
#include "umicom/ui/event.h"
#include "umicom/ui/explorer.h"
#include "umicom/ui/keybinding.h"
#include "umicom/ui/layout.h"
#include "umicom/ui/menu.h"
#include "umicom/ui/notification.h"
#include "umicom/ui/pane.h"
#include "umicom/ui/perspective.h"
#include "umicom/ui/quick_access.h"
#include "umicom/ui/selection.h"
#include "umicom/ui/status.h"
#include "umicom/ui/toolbar.h"
#include "umicom/ui/view_container.h"
#include "umicom/ui/view_factory.h"
#include "umicom/ui/workbench_state.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiWorkbenchSnapshot {
    char workbench_id[UMI_UI_ID_CAPACITY];
    char active_perspective[UMI_UI_ID_CAPACITY];
    char active_document_view[UMI_UI_ID_CAPACITY];
    char active_activity[UMI_UI_ID_CAPACITY];
    char active_view_container[UMI_UI_ID_CAPACITY];
    size_t pane_count;
    size_t document_count;
    size_t perspective_count;
    size_t action_count;
    size_t notification_count;
    size_t activity_count;
    size_t view_container_count;
    size_t keybinding_count;
    size_t context_key_count;
    size_t explorer_node_count;
    uint64_t revision;
} UmiUiWorkbenchSnapshot;

typedef struct UmiUiWorkbench UmiUiWorkbench;

UmiStatus umi_ui_workbench_create(const char *workbench_id,
                                  UmiCommandRegistry *commands,
                                  UmiUiWorkbench **out_workbench);
void umi_ui_workbench_destroy(UmiUiWorkbench *workbench);

UmiStatus umi_ui_workbench_activate_perspective(UmiUiWorkbench *workbench,
                                                const char *perspective_id);
UmiStatus umi_ui_workbench_activate_document(UmiUiWorkbench *workbench,
                                             const char *view_id);
UmiStatus umi_ui_workbench_activate_activity(UmiUiWorkbench *workbench,
                                             const char *activity_id);
UmiStatus umi_ui_workbench_execute_action(UmiUiWorkbench *workbench,
                                          const char *action_id,
                                          const char *argument,
                                          char *out_message,
                                          size_t capacity);
UmiStatus umi_ui_workbench_resolve_keybinding(
    UmiUiWorkbench *workbench,
    const char *chord,
    UmiUiKeybindingResolution *out_resolution);
UmiStatus umi_ui_workbench_quick_access(
    UmiUiWorkbench *workbench,
    const char *query,
    UmiUiQuickAccessResults *out_results);
UmiStatus umi_ui_workbench_set_breadcrumb_path(UmiUiWorkbench *workbench,
                                               const char *path);

UmiStatus umi_ui_workbench_snapshot(const UmiUiWorkbench *workbench,
                                    UmiUiWorkbenchSnapshot *out_snapshot);
UmiStatus umi_ui_workbench_state_snapshot(const UmiUiWorkbench *workbench,
                                          UmiUiWorkbenchState *out_state);
UmiStatus umi_ui_workbench_state_apply(UmiUiWorkbench *workbench,
                                       const UmiUiWorkbenchState *state);

UmiCommandRegistry *umi_ui_workbench_commands(UmiUiWorkbench *workbench);
UmiUiPaneModel *umi_ui_workbench_panes(UmiUiWorkbench *workbench);
UmiUiDocumentViewModel *umi_ui_workbench_documents(UmiUiWorkbench *workbench);
UmiUiEditorModel *umi_ui_workbench_editors(UmiUiWorkbench *workbench);
UmiUiPerspectiveModel *umi_ui_workbench_perspectives(UmiUiWorkbench *workbench);
UmiUiLayout *umi_ui_workbench_layout(UmiUiWorkbench *workbench);
UmiUiActionModel *umi_ui_workbench_actions(UmiUiWorkbench *workbench);
UmiUiMenuModel *umi_ui_workbench_menus(UmiUiWorkbench *workbench);
UmiUiToolbarModel *umi_ui_workbench_toolbars(UmiUiWorkbench *workbench);
UmiUiStatusModel *umi_ui_workbench_status(UmiUiWorkbench *workbench);
UmiUiSelectionModel *umi_ui_workbench_selection(UmiUiWorkbench *workbench);
UmiUiNotificationCentre *umi_ui_workbench_notifications(UmiUiWorkbench *workbench);
UmiUiDialogService *umi_ui_workbench_dialogs(UmiUiWorkbench *workbench);
UmiUiContributionModel *umi_ui_workbench_contributions(UmiUiWorkbench *workbench);
UmiUiBindingRegistry *umi_ui_workbench_bindings(UmiUiWorkbench *workbench);
UmiUiEventQueue *umi_ui_workbench_events(UmiUiWorkbench *workbench);
UmiUiViewFactoryRegistry *umi_ui_workbench_view_factories(UmiUiWorkbench *workbench);

/* Batch 23 workbench services. */
UmiUiActivityModel *umi_ui_workbench_activities(UmiUiWorkbench *workbench);
UmiUiViewContainerModel *umi_ui_workbench_view_containers(UmiUiWorkbench *workbench);
UmiUiContextStore *umi_ui_workbench_context(UmiUiWorkbench *workbench);
UmiUiKeybindingRegistry *umi_ui_workbench_keybindings(UmiUiWorkbench *workbench);
UmiUiBreadcrumbModel *umi_ui_workbench_breadcrumbs(UmiUiWorkbench *workbench);
UmiUiExplorerModel *umi_ui_workbench_explorer(UmiUiWorkbench *workbench);

#ifdef __cplusplus
}
#endif
#endif
