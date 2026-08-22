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

static UmiStatus interface_run(
    void *instance,
    int argc,
    char **argv,
    int *out_exit_code)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)instance;
    int result;
    if (adapter == NULL || adapter->application == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    result = g_application_run(
        G_APPLICATION(adapter->application),
        argc,
        argv);
    if (out_exit_code != NULL) *out_exit_code = result;
    return UMI_STATUS_OK;
}
static void interface_destroy(void *instance)
{
    umi_gtk4_adapter_destroy((UmiGtk4Adapter *)instance);
}

UmiStatus umi_gtk4_adapter_create(
    void *native_application,
    UmiGtk4Adapter **out_adapter)
{
    UmiGtk4Adapter *adapter;
    if (native_application == NULL || out_adapter == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_adapter = NULL;
    adapter = (UmiGtk4Adapter *)calloc(1U, sizeof(*adapter));
    if (adapter == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    adapter->application = GTK_APPLICATION(native_application);
    adapter->context_interaction_sequence = 1U;
    *out_adapter = adapter;
    return UMI_STATUS_OK;
}

void umi_gtk4_adapter_destroy(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL) return;

    /*
     * Disable the borrowed interaction sink before destroying the GTK object
     * tree. Existing signal closures can therefore never call product
     * composition while window teardown is in progress.
     */
    umi_gtk4_context_interaction_unbind(adapter);

    if (adapter->window != NULL) {
        gtk_window_destroy(adapter->window);
    }
    if (adapter->theme_provider != NULL) {
        if (adapter->theme_display != NULL) {
            gtk_style_context_remove_provider_for_display(
                adapter->theme_display,
                GTK_STYLE_PROVIDER(adapter->theme_provider));
        }
        g_object_unref(adapter->theme_provider);
    }
    free(adapter);
}

UmiUiAdapter umi_gtk4_adapter_interface(UmiGtk4Adapter *adapter)
{
    UmiUiAdapter value;
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

UmiStatus umi_gtk4_adapter_present(
    UmiGtk4Adapter *adapter,
    UmiUiApplicationShell *shell)
{
    UmiStatus status;
    if (adapter == NULL || shell == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    adapter->shell = shell;
    if (adapter->window == NULL) {
        status = umi_gtk4_build_shell(adapter);
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_gtk4_adapter_refresh(adapter);
    if (status == UMI_STATUS_OK) {
        gtk_window_present(adapter->window);
    }
    return status;
}

UmiStatus umi_gtk4_adapter_bind_desktop_shell(
    UmiGtk4Adapter *adapter,
    UmiDesktopShellModel *desktop_shell)
{
    if (adapter == NULL || desktop_shell == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    adapter->desktop_shell = desktop_shell;
    if (adapter->window != NULL) {
        return umi_gtk4_refresh_desktop_shell(adapter);
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_gtk4_adapter_bind_context_interactions(
    UmiGtk4Adapter *adapter,
    const UmiGtk4ContextInteractionSink *sink)
{
    if (adapter == NULL || sink == NULL ||
        sink->structure_size != sizeof(*sink)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    adapter->context_interaction_sink = *sink;
    adapter->context_interactions_bound = 1;

    if (adapter->window != NULL && adapter->shell != NULL) {
        return umi_gtk4_context_interaction_refresh(adapter);
    }
    return UMI_STATUS_OK;
}

void umi_gtk4_adapter_unbind_context_interactions(
    UmiGtk4Adapter *adapter)
{
    if (adapter == NULL) return;
    umi_gtk4_context_interaction_unbind(adapter);
}

UmiStatus umi_gtk4_adapter_refresh(UmiGtk4Adapter *adapter)
{
    UmiUiApplicationShellSnapshot snapshot;
    UmiStatus status;
    if (adapter == NULL || adapter->shell == NULL ||
        adapter->window == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ui_application_shell_snapshot(
        adapter->shell,
        &snapshot);
    if (status != UMI_STATUS_OK) return status;

    gtk_window_set_title(adapter->window, snapshot.title);
    gtk_window_set_default_size(
        adapter->window,
        snapshot.bounds.width,
        snapshot.bounds.height);

    status = umi_gtk4_refresh_workbench(adapter);
    if (status != UMI_STATUS_OK) return status;

    if (adapter->context_interactions_bound) {
        status = umi_gtk4_context_interaction_refresh(adapter);
    }
    return status;
}

void *umi_gtk4_adapter_native_window(UmiGtk4Adapter *adapter)
{
    return adapter != NULL ? adapter->window : NULL;
}
