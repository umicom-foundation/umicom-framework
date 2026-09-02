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

/**
 * Represent the desk runtime config data shared with callers of this public contract.
 */
typedef struct UmiDeskRuntimeConfig {
    uint32_t structure_size;
    bool seed_framework_portfolio;
    UmiApplicationLaunchConfig launcher;
} UmiDeskRuntimeConfig;

/**
 * Represent the desk runtime snapshot data shared with callers of this public contract.
 */
typedef struct UmiDeskRuntimeSnapshot {
    UmiApplicationRuntimeSnapshot applications;
    UmiApplicationLauncherSnapshot launcher;
    UmiApplicationLaunchSelectionSnapshot launch_selection;
    UmiDesktopApplicationStripSnapshot strip;
    UmiDesktopShellSnapshot shell;
    bool has_shell;
    uint64_t revision;
} UmiDeskRuntimeSnapshot;

/**
 * Represent the desk runtime data shared with callers of this public contract.
 */
typedef struct UmiDeskRuntime UmiDeskRuntime;

/**
 * Provide the desk runtime config default operation used by this module and its client
 * applications.
 */
UmiDeskRuntimeConfig umi_desk_runtime_config_default(void);

/**
 * Initialise desk runtime from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_desk_runtime_create(
    UmiDesktopShellModel *shell_model,
    const UmiDeskRuntimeConfig *config,
    const UmiApplicationLauncherAdapter *launch_adapter,
    UmiDeskRuntime **out_runtime);
/**
 * Release or reset state held by desk runtime so the same storage can be reused safely.
 */
void umi_desk_runtime_destroy(UmiDeskRuntime *runtime);

/**
 * Provide the desk runtime upsert application operation used by this module and its client
 * applications.
 */
UmiStatus umi_desk_runtime_upsert_application(
    UmiDeskRuntime *runtime,
    const UmiApplicationRuntimeRegistration *registration);
/**
 * Provide the desk runtime set application presence operation used by this module and its
 * client applications.
 */
UmiStatus umi_desk_runtime_set_application_presence(
    UmiDeskRuntime *runtime,
    const char *application_id,
    bool installed,
    bool compatible,
    bool enabled);
/**
 * Provide the desk runtime request application operation used by this module and its
 * client applications.
 */
UmiStatus umi_desk_runtime_request_application(
    UmiDeskRuntime *runtime,
    const char *application_id,
    UmiDesktopApplicationStripAction action);
/**
 * Provide the desk runtime select application operation used by this module and its client
 * applications.
 */
UmiStatus umi_desk_runtime_select_application(
    UmiDeskRuntime *runtime,
    const char *application_id,
    bool selected);
/**
 * Provide the desk runtime select all applications operation used by this module and its
 * client applications.
 */
UmiStatus umi_desk_runtime_select_all_applications(
    UmiDeskRuntime *runtime);
/**
 * Provide the desk runtime clear application selection operation used by this module and
 * its client applications.
 */
UmiStatus umi_desk_runtime_clear_application_selection(
    UmiDeskRuntime *runtime);
/**
 * Provide the desk runtime launch selected applications operation used by this module and
 * its client applications.
 */
UmiStatus umi_desk_runtime_launch_selected_applications(
    UmiDeskRuntime *runtime,
    UmiApplicationLaunchSelectionReport *out_report);
/**
 * Provide the desk runtime reconcile application exit operation used by this module and
 * its client applications.
 */
UmiStatus umi_desk_runtime_reconcile_application_exit(
    UmiDeskRuntime *runtime,
    const char *application_id,
    int exit_code,
    const char *message);
/**
 * Provide the desk runtime activate layout operation used by this module and its client
 * applications.
 */
UmiStatus umi_desk_runtime_activate_layout(
    UmiDeskRuntime *runtime,
    const char *layout_id);
/**
 * Provide the desk runtime refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_desk_runtime_refresh(UmiDeskRuntime *runtime);
/**
 * Provide the desk runtime snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_desk_runtime_snapshot(
    const UmiDeskRuntime *runtime,
    UmiDeskRuntimeSnapshot *out_snapshot);

/**
 * Provide the desk runtime applications operation used by this module and its client
 * applications.
 */
UmiApplicationRuntimeCatalogue *umi_desk_runtime_applications(
    UmiDeskRuntime *runtime);
/**
 * Provide the desk runtime launcher operation used by this module and its client
 * applications.
 */
UmiApplicationLauncher *umi_desk_runtime_launcher(
    UmiDeskRuntime *runtime);
/**
 * Provide the desk runtime launch selection operation used by this module and its client
 * applications.
 */
UmiApplicationLaunchSelection *umi_desk_runtime_launch_selection(
    UmiDeskRuntime *runtime);
/**
 * Provide the desk runtime application strip operation used by this module and its client
 * applications.
 */
UmiDesktopApplicationStrip *umi_desk_runtime_application_strip(
    UmiDeskRuntime *runtime);
/**
 * Provide the desk runtime shell operation used by this module and its client
 * applications.
 */
UmiDesktopShellModel *umi_desk_runtime_shell(UmiDeskRuntime *runtime);

#ifdef __cplusplus
}
#endif

#endif
