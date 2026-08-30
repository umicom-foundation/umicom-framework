/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workbench_services.c
 *
 * PURPOSE:
 *   Implement the reusable operational workbench services shared by Studio and future Umicom desktop or web products.
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
#include "umicom/ui/workbench_services.h"

#include <stdlib.h>
#include <string.h>

struct UmiUiWorkbenchServices {
    UmiUiProblemRegistry *problem;
    UmiUiOutputChannelRegistry *output_channel;
    UmiUiProgressRegistry *progress;
    UmiUiTaskMonitorRegistry *task_monitor;
    UmiUiNotificationItemRegistry *notification_item;
    UmiUiStatusItemRegistry *status_item;
    UmiUiCommandHistoryRegistry *command_history;
    UmiUiNavigationEntryRegistry *navigation_stack;
    UmiUiViewStateRegistry *view_state;
    uint64_t revision;
};

UmiStatus umi_ui_workbench_services_create(UmiUiWorkbenchServices **out_owner)
{
    UmiUiWorkbenchServices *owner; UmiStatus status = UMI_STATUS_OK;
    if (out_owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_owner = NULL; owner = (UmiUiWorkbenchServices *)calloc(1U,sizeof(*owner));
    if (owner == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    owner->revision = 1U;
    if (status == UMI_STATUS_OK) status = umi_ui_problem_registry_create(&owner->problem);
    if (status == UMI_STATUS_OK) status = umi_ui_output_channel_registry_create(&owner->output_channel);
    if (status == UMI_STATUS_OK) status = umi_ui_progress_registry_create(&owner->progress);
    if (status == UMI_STATUS_OK) status = umi_ui_task_monitor_registry_create(&owner->task_monitor);
    if (status == UMI_STATUS_OK) status = umi_ui_notification_item_registry_create(&owner->notification_item);
    if (status == UMI_STATUS_OK) status = umi_ui_status_item_registry_create(&owner->status_item);
    if (status == UMI_STATUS_OK) status = umi_ui_command_history_registry_create(&owner->command_history);
    if (status == UMI_STATUS_OK) status = umi_ui_navigation_stack_registry_create(&owner->navigation_stack);
    if (status == UMI_STATUS_OK) status = umi_ui_view_state_registry_create(&owner->view_state);
    if (status != UMI_STATUS_OK) { umi_ui_workbench_services_destroy(owner); return status; }
    *out_owner = owner; return UMI_STATUS_OK;
}

void umi_ui_workbench_services_destroy(UmiUiWorkbenchServices *owner)
{
    if (owner == NULL) return;
    umi_ui_view_state_registry_destroy(owner->view_state);
    umi_ui_navigation_stack_registry_destroy(owner->navigation_stack);
    umi_ui_command_history_registry_destroy(owner->command_history);
    umi_ui_status_item_registry_destroy(owner->status_item);
    umi_ui_notification_item_registry_destroy(owner->notification_item);
    umi_ui_task_monitor_registry_destroy(owner->task_monitor);
    umi_ui_progress_registry_destroy(owner->progress);
    umi_ui_output_channel_registry_destroy(owner->output_channel);
    umi_ui_problem_registry_destroy(owner->problem);
    free(owner);
}

UmiStatus umi_ui_workbench_services_snapshot(const UmiUiWorkbenchServices *owner, UmiUiWorkbenchServicesSnapshot *out_snapshot)
{
    if (owner == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot,0,sizeof(*out_snapshot));
    out_snapshot->struct_size=(uint32_t)sizeof(*out_snapshot); out_snapshot->api_version=1U;
    out_snapshot->revision=owner->revision;
    out_snapshot->problem_count = umi_ui_problem_registry_count(owner->problem);
    out_snapshot->output_channel_count = umi_ui_output_channel_registry_count(owner->output_channel);
    out_snapshot->progress_count = umi_ui_progress_registry_count(owner->progress);
    out_snapshot->task_monitor_count = umi_ui_task_monitor_registry_count(owner->task_monitor);
    out_snapshot->notification_item_count = umi_ui_notification_item_registry_count(owner->notification_item);
    out_snapshot->status_item_count = umi_ui_status_item_registry_count(owner->status_item);
    out_snapshot->command_history_count = umi_ui_command_history_registry_count(owner->command_history);
    out_snapshot->navigation_stack_count = umi_ui_navigation_stack_registry_count(owner->navigation_stack);
    out_snapshot->view_state_count = umi_ui_view_state_registry_count(owner->view_state);
    out_snapshot->item_count = out_snapshot->problem_count + out_snapshot->output_channel_count + out_snapshot->progress_count + out_snapshot->task_monitor_count + out_snapshot->notification_item_count + out_snapshot->status_item_count + out_snapshot->command_history_count + out_snapshot->navigation_stack_count + out_snapshot->view_state_count;
    return UMI_STATUS_OK;
}

UmiUiProblemRegistry *umi_ui_workbench_services_problem(UmiUiWorkbenchServices *owner) { return owner != NULL ? owner->problem : NULL; }
UmiUiOutputChannelRegistry *umi_ui_workbench_services_output_channel(UmiUiWorkbenchServices *owner) { return owner != NULL ? owner->output_channel : NULL; }
UmiUiProgressRegistry *umi_ui_workbench_services_progress(UmiUiWorkbenchServices *owner) { return owner != NULL ? owner->progress : NULL; }
UmiUiTaskMonitorRegistry *umi_ui_workbench_services_task_monitor(UmiUiWorkbenchServices *owner) { return owner != NULL ? owner->task_monitor : NULL; }
UmiUiNotificationItemRegistry *umi_ui_workbench_services_notification_item(UmiUiWorkbenchServices *owner) { return owner != NULL ? owner->notification_item : NULL; }
UmiUiStatusItemRegistry *umi_ui_workbench_services_status_item(UmiUiWorkbenchServices *owner) { return owner != NULL ? owner->status_item : NULL; }
UmiUiCommandHistoryRegistry *umi_ui_workbench_services_command_history(UmiUiWorkbenchServices *owner) { return owner != NULL ? owner->command_history : NULL; }
UmiUiNavigationEntryRegistry *umi_ui_workbench_services_navigation_stack(UmiUiWorkbenchServices *owner) { return owner != NULL ? owner->navigation_stack : NULL; }
UmiUiViewStateRegistry *umi_ui_workbench_services_view_state(UmiUiWorkbenchServices *owner) { return owner != NULL ? owner->view_state : NULL; }
