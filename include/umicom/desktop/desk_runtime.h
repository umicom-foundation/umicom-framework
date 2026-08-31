/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/desk_runtime.h
 *
 * PURPOSE:
 *   Compose application discovery, launcher policy, application-taskbar state
 *   and the existing semantic desktop shell into one Master Controller-owned
 *   Umicom Desk runtime.
 *
 * OWNERSHIP:
 *   The Desk runtime owns its application catalogue, launcher and strip. A
 *   supplied UmiDesktopShellModel is borrowed and remains owned by its creator.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_DESK_RUNTIME_H
#define UMICOM_DESKTOP_DESK_RUNTIME_H

#include <stdbool.h>
#include <stdint.h>

#include "umicom/application/launcher.h"
#include "umicom/application/launch_selection.h"
#include "umicom/application/runtime_catalogue.h"
#include "umicom/desktop/application_strip.h"
#include "umicom/desktop/shell_model.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeskRuntimeConfig {
    uint32_t structure_size;
    bool seed_framework_portfolio;
    UmiApplicationLaunchConfig launcher;
} UmiDeskRuntimeConfig;

typedef struct UmiDeskRuntimeSnapshot {
    UmiApplicationRuntimeSnapshot applications;
    UmiApplicationLauncherSnapshot launcher;
    UmiApplicationLaunchSelectionSnapshot launch_selection;
    UmiDesktopApplicationStripSnapshot strip;
    UmiDesktopShellSnapshot shell;
    bool has_shell;
    uint64_t revision;
} UmiDeskRuntimeSnapshot;

typedef struct UmiDeskRuntime UmiDeskRuntime;

UmiDeskRuntimeConfig umi_desk_runtime_config_default(void);

UmiStatus umi_desk_runtime_create(
    UmiDesktopShellModel *shell_model,
    const UmiDeskRuntimeConfig *config,
    const UmiApplicationLauncherAdapter *launch_adapter,
    UmiDeskRuntime **out_runtime);
void umi_desk_runtime_destroy(UmiDeskRuntime *runtime);

UmiStatus umi_desk_runtime_upsert_application(
    UmiDeskRuntime *runtime,
    const UmiApplicationRuntimeRegistration *registration);
UmiStatus umi_desk_runtime_set_application_presence(
    UmiDeskRuntime *runtime,
    const char *application_id,
    bool installed,
    bool compatible,
    bool enabled);
UmiStatus umi_desk_runtime_request_application(
    UmiDeskRuntime *runtime,
    const char *application_id,
    UmiDesktopApplicationStripAction action);
UmiStatus umi_desk_runtime_select_application(
    UmiDeskRuntime *runtime,
    const char *application_id,
    bool selected);
UmiStatus umi_desk_runtime_select_all_applications(
    UmiDeskRuntime *runtime);
UmiStatus umi_desk_runtime_clear_application_selection(
    UmiDeskRuntime *runtime);
UmiStatus umi_desk_runtime_launch_selected_applications(
    UmiDeskRuntime *runtime,
    UmiApplicationLaunchSelectionReport *out_report);
UmiStatus umi_desk_runtime_reconcile_application_exit(
    UmiDeskRuntime *runtime,
    const char *application_id,
    int exit_code,
    const char *message);
UmiStatus umi_desk_runtime_activate_layout(
    UmiDeskRuntime *runtime,
    const char *layout_id);
UmiStatus umi_desk_runtime_refresh(UmiDeskRuntime *runtime);
UmiStatus umi_desk_runtime_snapshot(
    const UmiDeskRuntime *runtime,
    UmiDeskRuntimeSnapshot *out_snapshot);

UmiApplicationRuntimeCatalogue *umi_desk_runtime_applications(
    UmiDeskRuntime *runtime);
UmiApplicationLauncher *umi_desk_runtime_launcher(
    UmiDeskRuntime *runtime);
UmiApplicationLaunchSelection *umi_desk_runtime_launch_selection(
    UmiDeskRuntime *runtime);
UmiDesktopApplicationStrip *umi_desk_runtime_application_strip(
    UmiDeskRuntime *runtime);
UmiDesktopShellModel *umi_desk_runtime_shell(UmiDeskRuntime *runtime);

#ifdef __cplusplus
}
#endif

#endif
