/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/host_adapter.h
 *
 * PURPOSE:
 *   Define the minimal toolkit-host boundary for Umicom Studio. GTK4, Qt or
 *   another frontend renders Framework-owned state through these callbacks.
 *
 * IMPORTANT:
 *   The adapter contains presentation callbacks only. It does not implement IDE
 *   business logic, document ownership, commands, build, test, debug, VCS or AI.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_HOST_ADAPTER_H
#define UMICOM_STUDIO_RUNTIME_HOST_ADAPTER_H

#include "umicom/studio_runtime/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiStudioHostSetWindowTitle)(
    void *user_data,
    const char *title);

typedef UmiStatus (*UmiStudioHostPresentSurface)(
    void *user_data,
    const UmiApplicationShellState *shell_state);

typedef UmiStatus (*UmiStudioHostUpdateStatus)(
    void *user_data,
    const UmiStudioRuntimeStatusModel *status);

typedef UmiStatus (*UmiStudioHostUpdateDocumentTabs)(
    void *user_data,
    const UmiStudioRuntimeDocumentTabs *tabs);

typedef UmiStatus (*UmiStudioHostUpdateWorkflow)(
    void *user_data,
    const UmiIdeWorkflowReport *workflow);

typedef UmiStatus (*UmiStudioHostNotify)(
    void *user_data,
    const char *message);

typedef struct UmiStudioRuntimeHostAdapter {
    UmiStudioHostSetWindowTitle set_window_title;
    UmiStudioHostPresentSurface present_surface;
    UmiStudioHostUpdateStatus update_status;
    UmiStudioHostUpdateDocumentTabs update_document_tabs;
    UmiStudioHostUpdateWorkflow update_workflow;
    UmiStudioHostNotify notify;
    void *user_data;
} UmiStudioRuntimeHostAdapter;

UmiStatus umi_studio_host_adapter_validate(
    const UmiStudioRuntimeHostAdapter *adapter);

#ifdef __cplusplus
}
#endif
#endif
