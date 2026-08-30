/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workbench_services.h
 *
 * PURPOSE:
 *   Define the reusable operational workbench services shared by Studio and future Umicom desktop or web products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef FRAMEWORK_INCLUDE_UMICOM_UI_WORKBENCH_SERVICES_H
#define FRAMEWORK_INCLUDE_UMICOM_UI_WORKBENCH_SERVICES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/problem.h"
#include "umicom/ui/output_channel.h"
#include "umicom/ui/progress.h"
#include "umicom/ui/task_monitor.h"
#include "umicom/ui/notification_item.h"
#include "umicom/ui/status_item.h"
#include "umicom/ui/command_history.h"
#include "umicom/ui/navigation_stack.h"
#include "umicom/ui/view_state.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiWorkbenchServices UmiUiWorkbenchServices;
typedef struct UmiUiWorkbenchServicesSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t revision;
    size_t item_count;
    size_t problem_count;
    size_t output_channel_count;
    size_t progress_count;
    size_t task_monitor_count;
    size_t notification_item_count;
    size_t status_item_count;
    size_t command_history_count;
    size_t navigation_stack_count;
    size_t view_state_count;
} UmiUiWorkbenchServicesSnapshot;

UmiStatus umi_ui_workbench_services_create(UmiUiWorkbenchServices **out_owner);
void umi_ui_workbench_services_destroy(UmiUiWorkbenchServices *owner);
UmiStatus umi_ui_workbench_services_snapshot(const UmiUiWorkbenchServices *owner, UmiUiWorkbenchServicesSnapshot *out_snapshot);
UmiUiProblemRegistry *umi_ui_workbench_services_problem(UmiUiWorkbenchServices *owner);
UmiUiOutputChannelRegistry *umi_ui_workbench_services_output_channel(UmiUiWorkbenchServices *owner);
UmiUiProgressRegistry *umi_ui_workbench_services_progress(UmiUiWorkbenchServices *owner);
UmiUiTaskMonitorRegistry *umi_ui_workbench_services_task_monitor(UmiUiWorkbenchServices *owner);
UmiUiNotificationItemRegistry *umi_ui_workbench_services_notification_item(UmiUiWorkbenchServices *owner);
UmiUiStatusItemRegistry *umi_ui_workbench_services_status_item(UmiUiWorkbenchServices *owner);
UmiUiCommandHistoryRegistry *umi_ui_workbench_services_command_history(UmiUiWorkbenchServices *owner);
UmiUiNavigationEntryRegistry *umi_ui_workbench_services_navigation_stack(UmiUiWorkbenchServices *owner);
UmiUiViewStateRegistry *umi_ui_workbench_services_view_state(UmiUiWorkbenchServices *owner);

#ifdef __cplusplus
}
#endif
#endif
