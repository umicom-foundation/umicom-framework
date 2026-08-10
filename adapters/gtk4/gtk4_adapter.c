/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/gtk4_adapter.c
 *
 * PURPOSE:
 *   Implement GTK4 adapter lifecycle, stable function-table exposure and native
 *   window access.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4.h"

#include <stdlib.h>
#include <string.h>

#include "gtk4_internal.h"

static UmiStatus interface_present(void *instance, UmiUiApplicationShell *shell)
{
    return umi_gtk4_adapter_present((UmiGtk4Adapter *)instance, shell);
}
static UmiStatus interface_refresh(void *instance)
{
    return umi_gtk4_adapter_refresh((UmiGtk4Adapter *)instance);
}
static UmiStatus interface_run(void *instance, int argc, char **argv, int *out_exit_code)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)instance;
    int result;
    if (adapter == NULL || adapter->application == NULL) return UMI_STATUS_INVALID_STATE;
    result = g_application_run(G_APPLICATION(adapter->application), argc, argv);
    if (out_exit_code != NULL) *out_exit_code = result;
    return UMI_STATUS_OK;
}
static void interface_destroy(void *instance)
{
    umi_gtk4_adapter_destroy((UmiGtk4Adapter *)instance);
}

UmiStatus umi_gtk4_adapter_create(void *native_application,
                                  UmiGtk4Adapter **out_adapter)
{
    UmiGtk4Adapter *adapter;
    if (native_application == NULL || out_adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_adapter = NULL;
    adapter = (UmiGtk4Adapter *)calloc(1U, sizeof(*adapter));
    if (adapter == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    adapter->application = GTK_APPLICATION(native_application);
    *out_adapter = adapter;
    return UMI_STATUS_OK;
}

void umi_gtk4_adapter_destroy(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL) return;
    if (adapter->window != NULL) gtk_window_destroy(adapter->window);
    free(adapter);
}

UmiUiAdapterV1 umi_gtk4_adapter_interface(UmiGtk4Adapter *adapter)
{
    UmiUiAdapterV1 value;
    (void)memset(&value, 0, sizeof(value));
    value.structure_size = (uint32_t)sizeof(value);
    value.abi_version = UMI_UI_ADAPTER_ABI_VERSION;
    value.adapter_id = "umicom.ui.gtk4";
    value.display_name = "Umicom GTK4 UI";
    value.instance = adapter;
    value.present = interface_present;
    value.refresh = interface_refresh;
    value.run = interface_run;
    value.destroy = interface_destroy;
    return value;
}

UmiStatus umi_gtk4_adapter_present(UmiGtk4Adapter *adapter,
                                   UmiUiApplicationShell *shell)
{
    UmiStatus status;
    if (adapter == NULL || shell == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    adapter->shell = shell;
    if (adapter->window == NULL) {
        status = umi_gtk4_build_shell(adapter);
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_gtk4_adapter_refresh(adapter);
    if (status == UMI_STATUS_OK) gtk_window_present(adapter->window);
    return status;
}

UmiStatus umi_gtk4_adapter_refresh(UmiGtk4Adapter *adapter)
{
    UmiUiApplicationShellSnapshot snapshot;
    UmiStatus status;
    if (adapter == NULL || adapter->shell == NULL || adapter->window == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_ui_application_shell_snapshot(adapter->shell, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    gtk_window_set_title(adapter->window, snapshot.title);
    gtk_window_set_default_size(adapter->window,
                                snapshot.bounds.width,
                                snapshot.bounds.height);
    return umi_gtk4_refresh_workbench(adapter);
}

void *umi_gtk4_adapter_native_window(UmiGtk4Adapter *adapter)
{
    return adapter != NULL ? adapter->window : NULL;
}
