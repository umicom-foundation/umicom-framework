/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workbench.h
 *
 * PURPOSE:
 *   Define the authoritative toolkit-neutral workbench aggregate including activities, view containers, context, keybindings, quick access, Explorer and breadcrumbs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Products compose these Framework workbench models and frontend adapters
 * render them instead of creating a competing application shell.
 */

#ifndef UMICOM_UI_WORKBENCH_H
#define UMICOM_UI_WORKBENCH_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/runtime/command_registry.h"
#include "umicom/ui/action.h"
#include "umicom/ui/appearance.h"
#include "umicom/ui/activity.h"
#include "umicom/ui/binding.h"
#include "umicom/ui/breadcrumb.h"
#include "umicom/ui/context.h"
#include "umicom/ui/contribution.h"
#include "umicom/ui/dialog.h"
#include "umicom/ui/document_view.h"
#include "umicom/ui/document_layout.h"
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
#include "umicom/ui/workspace_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui workbench snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiWorkbenchSnapshot {
    char workbench_id[UMI_UI_ID_CAPACITY];
    char active_workspace_profile[UMI_UI_ID_CAPACITY];
    char active_perspective[UMI_UI_ID_CAPACITY];
    char active_document_view[UMI_UI_ID_CAPACITY];
    char active_activity[UMI_UI_ID_CAPACITY];
    char active_view_container[UMI_UI_ID_CAPACITY];
    size_t pane_count;
    size_t document_count;
    size_t perspective_count;
    size_t workspace_profile_count;
    size_t appearance_profile_count;
    size_t action_count;
    size_t notification_count;
    size_t activity_count;
    size_t view_container_count;
    size_t keybinding_count;
    size_t context_key_count;
    size_t explorer_node_count;
    uint64_t revision;
} UmiUiWorkbenchSnapshot;

/**
 * Represent the ui workbench data shared with callers of this public contract.
 */
typedef struct UmiUiWorkbench UmiUiWorkbench;

/**
 * Initialise ui workbench from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_workbench_create(const char *workbench_id,
                                  UmiCommandRegistry *commands,
                                  UmiUiWorkbench **out_workbench);
/**
 * Release or reset state held by ui workbench so the same storage can be reused safely.
 */
void umi_ui_workbench_destroy(UmiUiWorkbench *workbench);

/**
 * Provide the ui workbench activate perspective operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workbench_activate_perspective(UmiUiWorkbench *workbench,
                                                const char *perspective_id);
/**
 * Provide the ui workbench activate document operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_activate_document(UmiUiWorkbench *workbench,
                                             const char *view_id);
/**
 * Provide the ui workbench activate activity operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_activate_activity(UmiUiWorkbench *workbench,
                                             const char *activity_id);
/**
 * Provide the ui workbench activate workspace profile operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workbench_activate_workspace_profile(
    UmiUiWorkbench *workbench,
    const char *profile_id);
/**
 * Provide the ui workbench save workspace profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workbench_save_workspace_profile(
    UmiUiWorkbench *workbench,
    const char *label,
    const char *description,
    char *out_profile_id,
    size_t capacity);
/**
 * Provide the ui workbench update workspace profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workbench_update_workspace_profile(
    UmiUiWorkbench *workbench,
    const char *profile_id,
    const char *label,
    const char *description);
/**
 * Provide the ui workbench set workspace profile locked operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workbench_set_workspace_profile_locked(
    UmiUiWorkbench *workbench,
    const char *profile_id,
    int locked);
/**
 * Provide the ui workbench remove workspace profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workbench_remove_workspace_profile(
    UmiUiWorkbench *workbench,
    const char *profile_id);
/**
 * Provide the ui workbench dock pane operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_dock_pane(
    UmiUiWorkbench *workbench,
    const char *pane_id,
    UmiUiPlacement placement,
    int32_t order);
/**
 * Provide the ui workbench workspace profile modified operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workbench_workspace_profile_modified(
    UmiUiWorkbench *workbench,
    const char *profile_id,
    int *out_modified);
/**
 * Provide the ui workbench execute action operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_execute_action(UmiUiWorkbench *workbench,
                                          const char *action_id,
                                          const char *argument,
                                          char *out_message,
                                          size_t capacity);
/**
 * Provide the ui workbench resolve keybinding operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_resolve_keybinding(
    UmiUiWorkbench *workbench,
    const char *chord,
    UmiUiKeybindingResolution *out_resolution);
/**
 * Provide the ui workbench quick access operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_quick_access(
    UmiUiWorkbench *workbench,
    const char *query,
    UmiUiQuickAccessResults *out_results);
/**
 * Provide the ui workbench set breadcrumb path operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workbench_set_breadcrumb_path(UmiUiWorkbench *workbench,
                                               const char *path);

/**
 * Provide the ui workbench snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_snapshot(const UmiUiWorkbench *workbench,
                                    UmiUiWorkbenchSnapshot *out_snapshot);
/**
 * Provide the ui workbench state snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_state_snapshot(const UmiUiWorkbench *workbench,
                                          UmiUiWorkbenchState *out_state);
/**
 * Perform ui workbench state through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ui_workbench_state_apply(UmiUiWorkbench *workbench,
                                       const UmiUiWorkbenchState *state);

/**
 * Provide the ui workbench commands operation used by this module and its client
 * applications.
 */
UmiCommandRegistry *umi_ui_workbench_commands(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench panes operation used by this module and its client
 * applications.
 */
UmiUiPaneModel *umi_ui_workbench_panes(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench documents operation used by this module and its client
 * applications.
 */
UmiUiDocumentViewModel *umi_ui_workbench_documents(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench editors operation used by this module and its client
 * applications.
 */
UmiUiEditorModel *umi_ui_workbench_editors(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench perspectives operation used by this module and its client
 * applications.
 */
UmiUiPerspectiveModel *umi_ui_workbench_perspectives(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench workspace profiles operation used by this module and its client
 * applications.
 */
UmiUiWorkspaceProfileModel *umi_ui_workbench_workspace_profiles(
    UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench appearance operation used by this module and its client
 * applications.
 */
UmiUiAppearanceModel *umi_ui_workbench_appearance(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench layout operation used by this module and its client
 * applications.
 */
UmiUiLayout *umi_ui_workbench_layout(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench actions operation used by this module and its client
 * applications.
 */
UmiUiActionModel *umi_ui_workbench_actions(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench menus operation used by this module and its client
 * applications.
 */
UmiUiMenuModel *umi_ui_workbench_menus(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench toolbars operation used by this module and its client
 * applications.
 */
UmiUiToolbarModel *umi_ui_workbench_toolbars(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench status operation used by this module and its client
 * applications.
 */
UmiUiStatusModel *umi_ui_workbench_status(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench selection operation used by this module and its client
 * applications.
 */
UmiUiSelectionModel *umi_ui_workbench_selection(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench notifications operation used by this module and its client
 * applications.
 */
UmiUiNotificationCentre *umi_ui_workbench_notifications(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench dialogs operation used by this module and its client
 * applications.
 */
UmiUiDialogService *umi_ui_workbench_dialogs(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench contributions operation used by this module and its client
 * applications.
 */
UmiUiContributionModel *umi_ui_workbench_contributions(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench bindings operation used by this module and its client
 * applications.
 */
UmiUiBindingRegistry *umi_ui_workbench_bindings(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench events operation used by this module and its client
 * applications.
 */
UmiUiEventQueue *umi_ui_workbench_events(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench view factories operation used by this module and its client
 * applications.
 */
UmiUiViewFactoryRegistry *umi_ui_workbench_view_factories(UmiUiWorkbench *workbench);

/* Reusable workbench services for product compositions. */
UmiUiActivityModel *umi_ui_workbench_activities(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench view containers operation used by this module and its client
 * applications.
 */
UmiUiViewContainerModel *umi_ui_workbench_view_containers(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench context operation used by this module and its client
 * applications.
 */
UmiUiContextStore *umi_ui_workbench_context(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench keybindings operation used by this module and its client
 * applications.
 */
UmiUiKeybindingRegistry *umi_ui_workbench_keybindings(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench breadcrumbs operation used by this module and its client
 * applications.
 */
UmiUiBreadcrumbModel *umi_ui_workbench_breadcrumbs(UmiUiWorkbench *workbench);
/**
 * Provide the ui workbench explorer operation used by this module and its client
 * applications.
 */
UmiUiExplorerModel *umi_ui_workbench_explorer(UmiUiWorkbench *workbench);

#ifdef __cplusplus
}
#endif
#endif
