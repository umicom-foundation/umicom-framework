/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/gtk4_adapter.c
 *
 * PURPOSE:
 *   Implement GTK4 adapter lifecycle, stable function-table exposure and native
 *   window access.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4.h"

#include <stdlib.h>
#include <string.h>

#include "gtk4_internal.h"

/* Provide the interface present operation used by this module and its client applications. */
static UmiStatus interface_present(void *instance, UmiUiApplicationShell *shell)
{
    return umi_gtk4_adapter_present((UmiGtk4Adapter *)instance, shell);
}
/* Provide the interface refresh operation used by this module and its client applications. */
static UmiStatus interface_refresh(void *instance)
{
    return umi_gtk4_adapter_refresh((UmiGtk4Adapter *)instance);
}

/*
 * Perform interface through the module contract so client applications do not duplicate
 * its policy.
 */
static UmiStatus interface_run(
    void *instance,
    int argc,
    char **argv,
    int *out_exit_code)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)instance;
    int result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->application == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    result = g_application_run(
        G_APPLICATION(adapter->application),
        argc,
        argv);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_exit_code != NULL) *out_exit_code = result;
    return UMI_STATUS_OK;
}
/* Release or reset state held by interface so the same storage can be reused safely. */
static void interface_destroy(void *instance)
{
    umi_gtk4_adapter_destroy((UmiGtk4Adapter *)instance);
}

/*
 * Initialise gtk4 adapter from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_gtk4_adapter_create(
    void *native_application,
    UmiGtk4Adapter **out_adapter)
{
    UmiGtk4Adapter *adapter;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (native_application == NULL || out_adapter == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_adapter = NULL;
    adapter = (UmiGtk4Adapter *)calloc(1U, sizeof(*adapter));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    adapter->application = GTK_APPLICATION(native_application);
    adapter->context_interaction_sequence = 1U;
    *out_adapter = adapter;
    return UMI_STATUS_OK;
}

/* Release or reset state held by gtk4 adapter so the same storage can be reused safely. */
void umi_gtk4_adapter_destroy(UmiGtk4Adapter *adapter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL) return;

    /*
     * Disable the borrowed interaction sink before destroying the GTK object
     * tree. Existing signal closures can therefore never call product
     * composition while window teardown is in progress.
     */
    umi_gtk4_context_interaction_unbind(adapter);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter->window != NULL) {
        gtk_window_destroy(adapter->window);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter->theme_provider != NULL) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (adapter->theme_display != NULL) {
            gtk_style_context_remove_provider_for_display(
                adapter->theme_display,
                GTK_STYLE_PROVIDER(adapter->theme_provider));
        }
        g_object_unref(adapter->theme_provider);
    }
    free(adapter);
}

/*
 * Provide the gtk4 adapter interface operation used by this module and its client
 * applications.
 */
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

/*
 * Provide the gtk4 adapter present operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_adapter_present(
    UmiGtk4Adapter *adapter,
    UmiUiApplicationShell *shell)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || shell == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    adapter->shell = shell;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter->window == NULL) {
        status = umi_gtk4_build_shell(adapter);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_gtk4_adapter_refresh(adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        gtk_window_present(adapter->window);
    }
    return status;
}

/*
 * Provide the gtk4 adapter bind desktop shell operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_adapter_bind_desktop_shell(
    UmiGtk4Adapter *adapter,
    UmiDesktopShellModel *desktop_shell)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || desktop_shell == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    adapter->desktop_shell = desktop_shell;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter->window != NULL) {
        return umi_gtk4_refresh_desktop_shell(adapter);
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the gtk4 adapter bind context interactions operation used by this module and its
 * client applications.
 */
UmiStatus umi_gtk4_adapter_bind_context_interactions(
    UmiGtk4Adapter *adapter,
    const UmiGtk4ContextInteractionSink *sink)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || sink == NULL ||
        sink->structure_size != sizeof(*sink)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    adapter->context_interaction_sink = *sink;
    adapter->context_interactions_bound = 1;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter->window != NULL && adapter->shell != NULL) {
        return umi_gtk4_context_interaction_refresh(adapter);
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the gtk4 adapter unbind context interactions operation used by this module and
 * its client applications.
 */
void umi_gtk4_adapter_unbind_context_interactions(
    UmiGtk4Adapter *adapter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL) return;
    umi_gtk4_context_interaction_unbind(adapter);
}

/*
 * Provide the gtk4 adapter refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_adapter_refresh(UmiGtk4Adapter *adapter)
{
    UmiUiApplicationShellSnapshot snapshot;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->shell == NULL ||
        adapter->window == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ui_application_shell_snapshot(
        adapter->shell,
        &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    gtk_window_set_title(adapter->window, snapshot.title);
    gtk_window_set_default_size(
        adapter->window,
        snapshot.bounds.width,
        snapshot.bounds.height);

    status = umi_gtk4_refresh_workbench(adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (adapter->context_interactions_bound) {
        status = umi_gtk4_context_interaction_refresh(adapter);
    }
    return status;
}

/*
 * Provide the gtk4 adapter native window operation used by this module and its client
 * applications.
 */
void *umi_gtk4_adapter_native_window(UmiGtk4Adapter *adapter)
{
    return adapter != NULL ? adapter->window : NULL;
}
