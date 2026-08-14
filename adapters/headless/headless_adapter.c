/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/headless_adapter.c
 *
 * PURPOSE:
 *   Implement lifecycle, adapter-interface bridging, refresh and snapshot operations
 *   for the reference headless frontend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/headless.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headless_internal.h"

UmiStatus umi_headless_append(UmiUiHeadlessAdapter *adapter,
                              const char *format,
                              ...)
{
    va_list arguments;
    int written;
    size_t remaining;
    if (adapter == NULL || format == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->length >= sizeof(adapter->render)) return UMI_STATUS_CAPACITY_EXCEEDED;
    remaining = sizeof(adapter->render) - adapter->length;
    va_start(arguments, format);
    written = vsnprintf(adapter->render + adapter->length,
                        remaining,
                        format,
                        arguments);
    va_end(arguments);
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    if ((size_t)written >= remaining) return UMI_STATUS_CAPACITY_EXCEEDED;
    adapter->length += (size_t)written;
    return UMI_STATUS_OK;
}

static UmiStatus interface_present(void *instance, UmiUiApplicationShell *shell)
{
    return umi_ui_headless_adapter_present((UmiUiHeadlessAdapter *)instance, shell);
}
static UmiStatus interface_refresh(void *instance)
{
    return umi_ui_headless_adapter_refresh((UmiUiHeadlessAdapter *)instance);
}
static UmiStatus interface_run(void *instance, int argc, char **argv, int *out_exit_code)
{
    UmiStatus status;
    (void)argc;
    (void)argv;
    status = umi_ui_headless_adapter_refresh((UmiUiHeadlessAdapter *)instance);
    if (out_exit_code != NULL) *out_exit_code = status == UMI_STATUS_OK ? 0 : 1;
    return status;
}
static void interface_destroy(void *instance)
{
    umi_ui_headless_adapter_destroy((UmiUiHeadlessAdapter *)instance);
}

UmiStatus umi_ui_headless_adapter_create(UmiUiHeadlessAdapter **out_adapter)
{
    UmiUiHeadlessAdapter *adapter;
    UmiStatus status;
    if (out_adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_adapter = NULL;
    adapter = (UmiUiHeadlessAdapter *)calloc(1U, sizeof(*adapter));
    if (adapter == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&adapter->mutex);
    if (status != UMI_STATUS_OK) {
        free(adapter);
        return status;
    }
    adapter->revision = 1U;
    *out_adapter = adapter;
    return UMI_STATUS_OK;
}

void umi_ui_headless_adapter_destroy(UmiUiHeadlessAdapter *adapter)
{
    if (adapter == NULL) return;
    umi_mutex_destroy(adapter->mutex);
    free(adapter);
}

UmiUiAdapter umi_ui_headless_adapter_interface(UmiUiHeadlessAdapter *adapter)
{
    UmiUiAdapter interface_value;
    (void)memset(&interface_value, 0, sizeof(interface_value));
    interface_value.structure_size = (uint32_t)sizeof(interface_value);
    interface_value.abi_version = UMI_UI_ADAPTER_ABI_VERSION;
    interface_value.adapter_id = "umicom.ui.headless";
    interface_value.display_name = "Umicom Headless UI";
    interface_value.instance = adapter;
    interface_value.present = interface_present;
    interface_value.refresh = interface_refresh;
    interface_value.run = interface_run;
    interface_value.destroy = interface_destroy;
    return interface_value;
}

UmiStatus umi_ui_headless_adapter_present(UmiUiHeadlessAdapter *adapter,
                                          UmiUiApplicationShell *shell)
{
    if (adapter == NULL || shell == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(adapter->mutex);
    adapter->shell = shell;
    (void)umi_mutex_unlock(adapter->mutex);
    return umi_ui_headless_adapter_refresh(adapter);
}

UmiStatus umi_ui_headless_adapter_refresh(UmiUiHeadlessAdapter *adapter)
{
    UmiStatus status;
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(adapter->mutex);
    if (adapter->shell == NULL) {
        (void)umi_mutex_unlock(adapter->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    adapter->length = 0U;
    adapter->render[0] = '\0';
    adapter->pane_count = 0U;
    adapter->document_count = 0U;
    adapter->notification_count = 0U;
    status = umi_headless_render_shell(adapter);
    if (status == UMI_STATUS_OK) adapter->revision += 1U;
    (void)umi_mutex_unlock(adapter->mutex);
    return status;
}

UmiStatus umi_ui_headless_adapter_snapshot(const UmiUiHeadlessAdapter *adapter,
                                           UmiUiHeadlessSnapshot *out_snapshot)
{
    if (adapter == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(adapter->mutex);
    out_snapshot->render_revision = adapter->revision;
    out_snapshot->pane_count = adapter->pane_count;
    out_snapshot->document_count = adapter->document_count;
    out_snapshot->notification_count = adapter->notification_count;
    (void)snprintf(out_snapshot->text, sizeof(out_snapshot->text), "%s", adapter->render);
    (void)umi_mutex_unlock(adapter->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_headless_adapter_complete_next_dialog(UmiUiHeadlessAdapter *adapter,
                                                        int accepted,
                                                        int32_t button_index,
                                                        const char *value)
{
    UmiUiDialogRequest request;
    UmiUiDialogResult result;
    UmiUiWorkbench *workbench;
    UmiStatus status;
    if (adapter == NULL || adapter->shell == NULL) return UMI_STATUS_INVALID_STATE;
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    status = umi_ui_dialog_next(umi_ui_workbench_dialogs(workbench), &request);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&result, 0, sizeof(result));
    result.request_id = request.request_id;
    result.accepted = accepted != 0;
    result.button_index = button_index;
    (void)snprintf(result.value, sizeof(result.value), "%s", value != NULL ? value : "");
    return umi_ui_dialog_complete(umi_ui_workbench_dialogs(workbench), &result);
}
