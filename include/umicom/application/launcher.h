/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/launcher.h
 *
 * PURPOSE:
 *   Convert governed Umicom Desk actions into immutable launch plans and route
 *   those plans through a platform adapter. Applications never execute sibling
 *   products directly and frontend adapters never construct shell commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_LAUNCHER_H
#define UMICOM_APPLICATION_LAUNCHER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/application/runtime_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_LAUNCH_MAX_ARGUMENTS 16U
#define UMI_APPLICATION_LAUNCH_ARGUMENT_CAPACITY 512U

/**
 * List the named application launch action values accepted by this public contract.
 */
typedef enum UmiApplicationLaunchAction {
    UMI_APPLICATION_LAUNCH_START = 1,
    UMI_APPLICATION_LAUNCH_ACTIVATE = 2,
    UMI_APPLICATION_LAUNCH_RESTART = 3,
    UMI_APPLICATION_LAUNCH_STOP = 4
} UmiApplicationLaunchAction;

/**
 * Represent the application launch config data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationLaunchConfig {
    uint32_t structure_size;
    const char *executable_root;
    const char *default_working_directory;
    const char *executable_suffix;
    uint32_t graceful_stop_timeout_ms;
} UmiApplicationLaunchConfig;

/**
 * Represent the application launch plan data shared with callers of this public contract.
 */
typedef struct UmiApplicationLaunchPlan {
    uint32_t structure_size;
    UmiApplicationLaunchAction action;
    char application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    char executable_path[UMI_APPLICATION_RUNTIME_PATH_CAPACITY];
    char working_directory[UMI_APPLICATION_RUNTIME_PATH_CAPACITY];
    char arguments[UMI_APPLICATION_LAUNCH_MAX_ARGUMENTS]
                  [UMI_APPLICATION_LAUNCH_ARGUMENT_CAPACITY];
    size_t argument_count;
    uint64_t existing_process_token;
    uint64_t plan_revision;
} UmiApplicationLaunchPlan;

typedef UmiStatus (*UmiApplicationLauncherStartFn)(
    void *adapter_context,
    const UmiApplicationLaunchPlan *plan,
    uint64_t *out_process_token);
typedef UmiStatus (*UmiApplicationLauncherActivateFn)(
    void *adapter_context,
    const char *application_id,
    uint64_t process_token);
typedef UmiStatus (*UmiApplicationLauncherStopFn)(
    void *adapter_context,
    const char *application_id,
    uint64_t process_token,
    uint32_t graceful_timeout_ms);

/**
 * Represent the application launcher adapter data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationLauncherAdapter {
    uint32_t structure_size;
    void *adapter_context;
    UmiApplicationLauncherStartFn start;
    UmiApplicationLauncherActivateFn activate;
    UmiApplicationLauncherStopFn stop;
} UmiApplicationLauncherAdapter;

/**
 * Represent the application launcher snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationLauncherSnapshot {
    UmiApplicationLaunchPlan last_plan;
    UmiStatus last_status;
    uint64_t request_count;
    uint64_t successful_request_count;
    uint64_t failed_request_count;
    uint64_t revision;
} UmiApplicationLauncherSnapshot;

/**
 * Represent the application launcher data shared with callers of this public contract.
 */
typedef struct UmiApplicationLauncher UmiApplicationLauncher;

/**
 * Provide the application launch config default operation used by this module and its
 * client applications.
 */
UmiApplicationLaunchConfig umi_application_launch_config_default(void);

/**
 * Initialise application launcher from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_application_launcher_create(
    UmiApplicationRuntimeCatalogue *catalogue,
    const UmiApplicationLaunchConfig *config,
    const UmiApplicationLauncherAdapter *adapter,
    UmiApplicationLauncher **out_launcher);
/**
 * Release or reset state held by application launcher so the same storage can be reused
 * safely.
 */
void umi_application_launcher_destroy(UmiApplicationLauncher *launcher);

/**
 * Provide the application launcher prepare operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_launcher_prepare(
    UmiApplicationLauncher *launcher,
    const char *application_id,
    UmiApplicationLaunchAction action,
    UmiApplicationLaunchPlan *out_plan);
/**
 * Perform application launcher through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_application_launcher_execute(
    UmiApplicationLauncher *launcher,
    const UmiApplicationLaunchPlan *plan);
/**
 * Provide the application launcher request operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_launcher_request(
    UmiApplicationLauncher *launcher,
    const char *application_id,
    UmiApplicationLaunchAction action);
/**
 * Provide the application launcher reconcile exit operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_launcher_reconcile_exit(
    UmiApplicationLauncher *launcher,
    const char *application_id,
    int exit_code,
    const char *message);
/**
 * Provide the application launcher snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_launcher_snapshot(
    const UmiApplicationLauncher *launcher,
    UmiApplicationLauncherSnapshot *out_snapshot);

/**
 * Provide the application launch action text operation used by this module and its client
 * applications.
 */
const char *umi_application_launch_action_text(
    UmiApplicationLaunchAction action);

#ifdef __cplusplus
}
#endif

#endif
