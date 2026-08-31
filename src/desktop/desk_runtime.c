/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/desk_runtime.c
 *
 * PURPOSE:
 *   Implement the unified Umicom Desk runtime controlled by the Framework
 *   Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/desk_runtime.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeskRuntime {
    UmiApplicationRuntimeCatalogue *applications;
    UmiApplicationLauncher *launcher;
    UmiApplicationLaunchSelection *launch_selection;
    UmiDesktopApplicationStrip *strip;
    UmiDesktopShellModel *shell;
    uint64_t revision;
};

static UmiDesktopApplicationState map_shell_state(
    UmiApplicationRuntimeState state)
{
    switch (state) {
    case UMI_APPLICATION_RUNTIME_UNAVAILABLE:
        return UMI_DESKTOP_APPLICATION_UNAVAILABLE;
    case UMI_APPLICATION_RUNTIME_STARTING:
        return UMI_DESKTOP_APPLICATION_STARTING;
    case UMI_APPLICATION_RUNTIME_RUNNING:
        return UMI_DESKTOP_APPLICATION_RUNNING;
    case UMI_APPLICATION_RUNTIME_ATTENTION:
        return UMI_DESKTOP_APPLICATION_ATTENTION;
    case UMI_APPLICATION_RUNTIME_FAILED:
        return UMI_DESKTOP_APPLICATION_FAILED;
    case UMI_APPLICATION_RUNTIME_STOPPING:
    case UMI_APPLICATION_RUNTIME_STOPPED:
    case UMI_APPLICATION_RUNTIME_UNKNOWN:
    default:
        return UMI_DESKTOP_APPLICATION_STOPPED;
    }
}

static UmiStatus synchronise_shell_application(
    UmiDeskRuntime *runtime,
    const char *application_id)
{
    UmiApplicationRuntimeRecord record;
    UmiStatus status;
    if (runtime == NULL || runtime->shell == NULL) return UMI_STATUS_OK;
    status = umi_application_runtime_catalogue_find(
        runtime->applications, application_id, &record);
    if (status != UMI_STATUS_OK) return status;

    status = umi_desktop_shell_model_set_application_presence(
        runtime->shell, application_id,
        record.installed, record.compatible, record.enabled);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;

    status = umi_desktop_shell_model_set_application_state(
        runtime->shell, application_id, map_shell_state(record.state));
    if (status != UMI_STATUS_OK) return status;
    status = umi_desktop_shell_model_pin_application(
        runtime->shell, application_id, record.pinned);
    if (status != UMI_STATUS_OK) return status;
    if (record.active) {
        status = umi_desktop_shell_model_activate_application(
            runtime->shell, application_id);
    }
    return status;
}

UmiDeskRuntimeConfig umi_desk_runtime_config_default(void)
{
    UmiDeskRuntimeConfig config;
    (void)memset(&config, 0, sizeof(config));
    config.structure_size = sizeof(config);
    config.seed_framework_portfolio = true;
    config.launcher = umi_application_launch_config_default();
    return config;
}

UmiStatus umi_desk_runtime_create(
    UmiDesktopShellModel *shell_model,
    const UmiDeskRuntimeConfig *config,
    const UmiApplicationLauncherAdapter *launch_adapter,
    UmiDeskRuntime **out_runtime)
{
    UmiDeskRuntimeConfig effective;
    UmiDeskRuntime *runtime;
    UmiStatus status;
    if (launch_adapter == NULL || out_runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_runtime = NULL;
    effective = config != NULL
        ? *config
        : umi_desk_runtime_config_default();
    if (effective.structure_size < sizeof(UmiDeskRuntimeConfig)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    runtime = (UmiDeskRuntime *)calloc(1U, sizeof(*runtime));
    if (runtime == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    runtime->shell = shell_model;
    runtime->revision = 1U;

    status = umi_application_runtime_catalogue_create(
        &runtime->applications);
    if (status == UMI_STATUS_OK &&
        effective.seed_framework_portfolio) {
        status = umi_application_runtime_catalogue_seed_portfolio(
            runtime->applications);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_application_launcher_create(
            runtime->applications, &effective.launcher,
            launch_adapter, &runtime->launcher);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_desktop_application_strip_create(
            runtime->applications, &runtime->strip);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_application_launch_selection_create(
            runtime->applications, &runtime->launch_selection);
    }
    if (status != UMI_STATUS_OK) {
        umi_desk_runtime_destroy(runtime);
        return status;
    }
    *out_runtime = runtime;
    return UMI_STATUS_OK;
}

void umi_desk_runtime_destroy(UmiDeskRuntime *runtime)
{
    if (runtime == NULL) return;
    umi_application_launch_selection_destroy(runtime->launch_selection);
    umi_desktop_application_strip_destroy(runtime->strip);
    umi_application_launcher_destroy(runtime->launcher);
    umi_application_runtime_catalogue_destroy(runtime->applications);
    free(runtime);
}

UmiStatus umi_desk_runtime_upsert_application(
    UmiDeskRuntime *runtime,
    const UmiApplicationRuntimeRegistration *registration)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_runtime_catalogue_upsert(
        runtime->applications, registration);
    if (status == UMI_STATUS_OK) {
        status = synchronise_shell_application(
            runtime, registration->application_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_desktop_application_strip_refresh(runtime->strip);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_application_launch_selection_refresh(
            runtime->launch_selection);
    }
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

UmiStatus umi_desk_runtime_set_application_presence(
    UmiDeskRuntime *runtime,
    const char *application_id,
    bool installed,
    bool compatible,
    bool enabled)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_runtime_catalogue_set_presence(
        runtime->applications, application_id,
        installed, compatible, enabled);
    if (status == UMI_STATUS_OK) {
        status = synchronise_shell_application(runtime, application_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_desktop_application_strip_refresh(runtime->strip);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_application_launch_selection_refresh(
            runtime->launch_selection);
    }
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

UmiStatus umi_desk_runtime_request_application(
    UmiDeskRuntime *runtime,
    const char *application_id,
    UmiDesktopApplicationStripAction action)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_application_strip_request(
        runtime->strip, runtime->launcher, application_id, action);
    if (status == UMI_STATUS_OK) {
        status = synchronise_shell_application(runtime, application_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_application_launch_selection_refresh(
            runtime->launch_selection);
    }
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

UmiStatus umi_desk_runtime_select_application(
    UmiDeskRuntime *runtime,
    const char *application_id,
    bool selected)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_launch_selection_set_selected(
        runtime->launch_selection, application_id, selected);
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

UmiStatus umi_desk_runtime_select_all_applications(
    UmiDeskRuntime *runtime)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_launch_selection_select_all(
        runtime->launch_selection);
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

UmiStatus umi_desk_runtime_clear_application_selection(
    UmiDeskRuntime *runtime)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_launch_selection_clear(
        runtime->launch_selection);
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

UmiStatus umi_desk_runtime_launch_selected_applications(
    UmiDeskRuntime *runtime,
    UmiApplicationLaunchSelectionReport *out_report)
{
    UmiStatus result;
    size_t index;
    if (runtime == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    result = umi_application_launch_selection_execute(
        runtime->launch_selection, runtime->launcher, out_report);

    /* Synchronise every attempted product, including partial failures. */
    for (index = 0U; index < out_report->result_count; ++index) {
        UmiStatus status = synchronise_shell_application(
            runtime, out_report->results[index].application_id);
        if (result == UMI_STATUS_OK && status != UMI_STATUS_OK) {
            result = status;
        }
    }
    {
        UmiStatus status = umi_desktop_application_strip_refresh(
            runtime->strip);
        if (result == UMI_STATUS_OK && status != UMI_STATUS_OK) {
            result = status;
        }
    }
    runtime->revision += 1U;
    return result;
}

UmiStatus umi_desk_runtime_reconcile_application_exit(
    UmiDeskRuntime *runtime,
    const char *application_id,
    int exit_code,
    const char *message)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_launcher_reconcile_exit(
        runtime->launcher, application_id, exit_code, message);
    if (status == UMI_STATUS_OK) {
        status = synchronise_shell_application(runtime, application_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_desktop_application_strip_refresh(runtime->strip);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_application_launch_selection_refresh(
            runtime->launch_selection);
    }
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

UmiStatus umi_desk_runtime_activate_layout(
    UmiDeskRuntime *runtime,
    const char *layout_id)
{
    UmiStatus status;
    if (runtime == NULL || runtime->shell == NULL) {
        return UMI_STATUS_UNAVAILABLE;
    }
    status = umi_desktop_shell_model_activate_layout(
        runtime->shell, layout_id);
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

UmiStatus umi_desk_runtime_refresh(UmiDeskRuntime *runtime)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_application_strip_refresh(runtime->strip);
    if (status == UMI_STATUS_OK) {
        status = umi_application_launch_selection_refresh(
            runtime->launch_selection);
    }
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

UmiStatus umi_desk_runtime_snapshot(
    const UmiDeskRuntime *runtime,
    UmiDeskRuntimeSnapshot *out_snapshot)
{
    UmiStatus status;
    if (runtime == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    status = umi_application_runtime_catalogue_snapshot(
        runtime->applications, &out_snapshot->applications);
    if (status == UMI_STATUS_OK) {
        status = umi_application_launcher_snapshot(
            runtime->launcher, &out_snapshot->launcher);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_application_launch_selection_snapshot(
            runtime->launch_selection, &out_snapshot->launch_selection);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_desktop_application_strip_snapshot(
            runtime->strip, &out_snapshot->strip);
    }
    if (status == UMI_STATUS_OK && runtime->shell != NULL) {
        status = umi_desktop_shell_model_snapshot(
            runtime->shell, &out_snapshot->shell);
        if (status == UMI_STATUS_OK) out_snapshot->has_shell = true;
    }
    out_snapshot->revision = runtime->revision;
    return status;
}

UmiApplicationRuntimeCatalogue *umi_desk_runtime_applications(
    UmiDeskRuntime *runtime)
{
    return runtime != NULL ? runtime->applications : NULL;
}

UmiApplicationLauncher *umi_desk_runtime_launcher(
    UmiDeskRuntime *runtime)
{
    return runtime != NULL ? runtime->launcher : NULL;
}

UmiApplicationLaunchSelection *umi_desk_runtime_launch_selection(
    UmiDeskRuntime *runtime)
{
    return runtime != NULL ? runtime->launch_selection : NULL;
}

UmiDesktopApplicationStrip *umi_desk_runtime_application_strip(
    UmiDeskRuntime *runtime)
{
    return runtime != NULL ? runtime->strip : NULL;
}

UmiDesktopShellModel *umi_desk_runtime_shell(UmiDeskRuntime *runtime)
{
    return runtime != NULL ? runtime->shell : NULL;
}
