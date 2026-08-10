/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workbench.h
 *
 * PURPOSE:
 *   Define the authoritative toolkit-neutral workbench state that composes panes,
 *   documents, editors, perspectives, layouts, selections and notifications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WORKBENCH_H
#define UMICOM_UI_WORKBENCH_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/runtime/command_registry.h"
#include "umicom/ui/action.h"
#include "umicom/ui/binding.h"
#include "umicom/ui/contribution.h"
#include "umicom/ui/dialog.h"
#include "umicom/ui/document_view.h"
#include "umicom/ui/editor.h"
#include "umicom/ui/event.h"
#include "umicom/ui/layout.h"
#include "umicom/ui/menu.h"
#include "umicom/ui/notification.h"
#include "umicom/ui/pane.h"
#include "umicom/ui/perspective.h"
#include "umicom/ui/selection.h"
#include "umicom/ui/status.h"
#include "umicom/ui/toolbar.h"
#include "umicom/ui/view_factory.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiUiWorkbenchSnapshot { char workbench_id[UMI_UI_ID_CAPACITY]; char active_perspective[UMI_UI_ID_CAPACITY]; char active_document_view[UMI_UI_ID_CAPACITY]; size_t pane_count; size_t document_count; size_t perspective_count; size_t action_count; size_t notification_count; uint64_t revision; } UmiUiWorkbenchSnapshot;
typedef struct UmiUiWorkbench UmiUiWorkbench;
UmiStatus umi_ui_workbench_create(const char *workbench_id, UmiCommandRegistry *commands, UmiUiWorkbench **out_workbench);
void umi_ui_workbench_destroy(UmiUiWorkbench *workbench);
UmiStatus umi_ui_workbench_activate_perspective(UmiUiWorkbench *workbench, const char *perspective_id);
UmiStatus umi_ui_workbench_activate_document(UmiUiWorkbench *workbench, const char *view_id);
UmiStatus umi_ui_workbench_execute_action(UmiUiWorkbench *workbench, const char *action_id, const char *argument, char *out_message, size_t capacity);
UmiStatus umi_ui_workbench_snapshot(const UmiUiWorkbench *workbench, UmiUiWorkbenchSnapshot *out_snapshot);
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

#ifdef __cplusplus
}
#endif

#endif
