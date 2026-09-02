/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/launcher.c
 *
 * PURPOSE:
 *   Implement governed application launch planning and adapter dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/launcher.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiApplicationLauncher {
    UmiApplicationRuntimeCatalogue *catalogue;
    UmiApplicationLauncherAdapter adapter;
    char executable_root[UMI_APPLICATION_RUNTIME_PATH_CAPACITY];
    char default_working_directory[UMI_APPLICATION_RUNTIME_PATH_CAPACITY];
    char executable_suffix[64U];
    uint32_t graceful_stop_timeout_ms;
    UmiApplicationLauncherSnapshot snapshot;
    uint64_t next_plan_revision;
};

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source,
                           bool allow_empty)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || source[0] == '\0') {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (!allow_empty) return UMI_STATUS_INVALID_ARGUMENT;
        destination[0] = '\0';
        return UMI_STATUS_OK;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the text ends with operation used by this module and its client applications. */
static bool text_ends_with(const char *text, const char *suffix)
{
    size_t text_length;
    size_t suffix_length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || suffix == NULL) return false;
    text_length = strlen(text);
    suffix_length = strlen(suffix);
    /* Apply this branch only when its contract condition is satisfied. */
    if (suffix_length == 0U || suffix_length > text_length) return false;
    return strcmp(text + text_length - suffix_length, suffix) == 0;
}

/*
 * Provide the make executable path operation used by this module and its client
 * applications.
 */
static UmiStatus make_executable_path(
    const UmiApplicationLauncher *launcher,
    const UmiApplicationRuntimeRecord *record,
    char *out_path,
    size_t capacity)
{
    const char *root;
    const char *suffix;
    bool has_root;
    bool has_suffix;
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (launcher == NULL || record == NULL || out_path == NULL ||
        record->executable_name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    root = launcher->executable_root;
    suffix = launcher->executable_suffix;
    has_root = root[0] != '\0';
    has_suffix = suffix[0] != '\0' &&
                 !text_ends_with(record->executable_name, suffix);
    /* Apply this branch only when its contract condition is satisfied. */
    if (has_root && has_suffix) {
        written = snprintf(out_path, capacity, "%s/%s%s",
                           root, record->executable_name, suffix);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (has_root) {
        written = snprintf(out_path, capacity, "%s/%s",
                           root, record->executable_name);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (has_suffix) {
        written = snprintf(out_path, capacity, "%s%s",
                           record->executable_name, suffix);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        written = snprintf(out_path, capacity, "%s",
                           record->executable_name);
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/* Provide the record request operation used by this module and its client applications. */
static void record_request(
    UmiApplicationLauncher *launcher,
    const UmiApplicationLaunchPlan *plan,
    UmiStatus status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (launcher == NULL) return;
    launcher->snapshot.request_count += 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        launcher->snapshot.successful_request_count += 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        launcher->snapshot.failed_request_count += 1U;
    }
    launcher->snapshot.last_status = status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan != NULL) launcher->snapshot.last_plan = *plan;
    launcher->snapshot.revision += 1U;
}

/*
 * Provide the application launch config default operation used by this module and its
 * client applications.
 */
UmiApplicationLaunchConfig umi_application_launch_config_default(void)
{
    UmiApplicationLaunchConfig config;
    (void)memset(&config, 0, sizeof(config));
    config.structure_size = sizeof(config);
    config.executable_root = "";
    config.default_working_directory = "";
#ifdef _WIN32
    config.executable_suffix = ".exe";
#else
    config.executable_suffix = "";
#endif
    config.graceful_stop_timeout_ms = 5000U;
    return config;
}

/*
 * Initialise application launcher from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_application_launcher_create(
    UmiApplicationRuntimeCatalogue *catalogue,
    const UmiApplicationLaunchConfig *config,
    const UmiApplicationLauncherAdapter *adapter,
    UmiApplicationLauncher **out_launcher)
{
    UmiApplicationLauncher *launcher;
    UmiApplicationLaunchConfig effective;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || adapter == NULL || out_launcher == NULL ||
        adapter->structure_size < sizeof(UmiApplicationLauncherAdapter) ||
        adapter->start == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_launcher = NULL;
    effective = config != NULL
        ? *config
        : umi_application_launch_config_default();
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.structure_size < sizeof(UmiApplicationLaunchConfig)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    launcher = (UmiApplicationLauncher *)calloc(1U, sizeof(*launcher));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (launcher == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    launcher->catalogue = catalogue;
    launcher->adapter = *adapter;
    launcher->graceful_stop_timeout_ms =
        effective.graceful_stop_timeout_ms;
    launcher->next_plan_revision = 1U;
    launcher->snapshot.last_status = UMI_STATUS_OK;
    launcher->snapshot.revision = 1U;

    status = copy_text(launcher->executable_root,
                       sizeof(launcher->executable_root),
                       effective.executable_root, true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(launcher->default_working_directory,
                           sizeof(launcher->default_working_directory),
                           effective.default_working_directory, true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(launcher->executable_suffix,
                           sizeof(launcher->executable_suffix),
                           effective.executable_suffix, true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(launcher);
        return status;
    }
    *out_launcher = launcher;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by application launcher so the same storage can be reused
 * safely.
 */
void umi_application_launcher_destroy(UmiApplicationLauncher *launcher)
{
    free(launcher);
}

/*
 * Provide the application launcher prepare operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_launcher_prepare(
    UmiApplicationLauncher *launcher,
    const char *application_id,
    UmiApplicationLaunchAction action,
    UmiApplicationLaunchPlan *out_plan)
{
    UmiApplicationRuntimeRecord record;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (launcher == NULL || application_id == NULL ||
        application_id[0] == '\0' || out_plan == NULL ||
        action < UMI_APPLICATION_LAUNCH_START ||
        action > UMI_APPLICATION_LAUNCH_STOP) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_application_runtime_catalogue_find(
        launcher->catalogue, application_id, &record);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!record.installed || !record.compatible ||
        !record.enabled || !record.visible) {
        return UMI_STATUS_UNAVAILABLE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (action == UMI_APPLICATION_LAUNCH_STOP && !record.running) {
        return UMI_STATUS_INVALID_STATE;
    }

    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->structure_size = sizeof(*out_plan);
    out_plan->action =
        action == UMI_APPLICATION_LAUNCH_START && record.running
            ? UMI_APPLICATION_LAUNCH_ACTIVATE
            : action;
    out_plan->existing_process_token = record.process_token;
    out_plan->plan_revision = launcher->next_plan_revision++;

    status = copy_text(out_plan->application_id,
                       sizeof(out_plan->application_id),
                       record.application_id, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = make_executable_path(launcher, &record,
                                      out_plan->executable_path,
                                      sizeof(out_plan->executable_path));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        const char *working_directory =
            record.working_directory[0] != '\0'
                ? record.working_directory
                : launcher->default_working_directory;
        status = copy_text(out_plan->working_directory,
                           sizeof(out_plan->working_directory),
                           working_directory, true);
    }
    return status;
}

/* Provide the execute start operation used by this module and its client applications. */
static UmiStatus execute_start(
    UmiApplicationLauncher *launcher,
    const UmiApplicationLaunchPlan *plan)
{
    uint64_t process_token = 0U;
    UmiStatus status;
    status = umi_application_runtime_catalogue_set_state(
        launcher->catalogue, plan->application_id,
        UMI_APPLICATION_RUNTIME_STARTING, "");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = launcher->adapter.start(
        launcher->adapter.adapter_context, plan, &process_token);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)umi_application_runtime_catalogue_set_state(
            launcher->catalogue, plan->application_id,
            UMI_APPLICATION_RUNTIME_FAILED,
            "Platform launch adapter rejected the start request.");
        return status;
    }
    status = umi_application_runtime_catalogue_set_process(
        launcher->catalogue, plan->application_id, process_token);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_application_runtime_catalogue_activate(
            launcher->catalogue, plan->application_id);
    }
    return status;
}

/* Provide the execute activate operation used by this module and its client applications. */
static UmiStatus execute_activate(
    UmiApplicationLauncher *launcher,
    const UmiApplicationLaunchPlan *plan)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (launcher->adapter.activate != NULL) {
        status = launcher->adapter.activate(
            launcher->adapter.adapter_context,
            plan->application_id,
            plan->existing_process_token);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_application_runtime_catalogue_activate(
            launcher->catalogue, plan->application_id);
    }
    return status;
}

/* Provide the execute stop operation used by this module and its client applications. */
static UmiStatus execute_stop(
    UmiApplicationLauncher *launcher,
    const UmiApplicationLaunchPlan *plan)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (launcher->adapter.stop == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    status = umi_application_runtime_catalogue_set_state(
        launcher->catalogue, plan->application_id,
        UMI_APPLICATION_RUNTIME_STOPPING, "");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = launcher->adapter.stop(
        launcher->adapter.adapter_context,
        plan->application_id,
        plan->existing_process_token,
        launcher->graceful_stop_timeout_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_application_runtime_catalogue_mark_exit(
            launcher->catalogue, plan->application_id, 0,
            "Stopped by Umicom Desk.");
    }
    return status;
}

/*
 * Perform application launcher through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_application_launcher_execute(
    UmiApplicationLauncher *launcher,
    const UmiApplicationLaunchPlan *plan)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (launcher == NULL || plan == NULL ||
        plan->structure_size < sizeof(UmiApplicationLaunchPlan)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Select the behaviour associated with the requested command or state value. */
    switch (plan->action) {
    case UMI_APPLICATION_LAUNCH_START:
        status = execute_start(launcher, plan);
        break;
    case UMI_APPLICATION_LAUNCH_ACTIVATE:
        status = execute_activate(launcher, plan);
        break;
    case UMI_APPLICATION_LAUNCH_STOP:
        status = execute_stop(launcher, plan);
        break;
    case UMI_APPLICATION_LAUNCH_RESTART: {
        UmiApplicationLaunchPlan start_plan = *plan;
        UmiApplicationRuntimeRecord record;
        status = umi_application_runtime_catalogue_find(
            launcher->catalogue, plan->application_id, &record);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK && record.running) {
            UmiApplicationLaunchPlan stop_plan = *plan;
            stop_plan.action = UMI_APPLICATION_LAUNCH_STOP;
            stop_plan.existing_process_token = record.process_token;
            status = execute_stop(launcher, &stop_plan);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            start_plan.action = UMI_APPLICATION_LAUNCH_START;
            start_plan.existing_process_token = 0U;
            status = execute_start(launcher, &start_plan);
        }
        break;
    }
    default:
        status = UMI_STATUS_INVALID_ARGUMENT;
        break;
    }
    record_request(launcher, plan, status);
    return status;
}

/*
 * Provide the application launcher request operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_launcher_request(
    UmiApplicationLauncher *launcher,
    const char *application_id,
    UmiApplicationLaunchAction action)
{
    UmiApplicationLaunchPlan plan;
    UmiStatus status = umi_application_launcher_prepare(
        launcher, application_id, action, &plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        record_request(launcher, NULL, status);
        return status;
    }
    return umi_application_launcher_execute(launcher, &plan);
}

/*
 * Provide the application launcher reconcile exit operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_launcher_reconcile_exit(
    UmiApplicationLauncher *launcher,
    const char *application_id,
    int exit_code,
    const char *message)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (launcher == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_runtime_catalogue_mark_exit(
        launcher->catalogue, application_id, exit_code, message);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) launcher->snapshot.revision += 1U;
    return status;
}

/*
 * Provide the application launcher snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_launcher_snapshot(
    const UmiApplicationLauncher *launcher,
    UmiApplicationLauncherSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (launcher == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_snapshot = launcher->snapshot;
    return UMI_STATUS_OK;
}

/*
 * Provide the application launch action text operation used by this module and its client
 * applications.
 */
const char *umi_application_launch_action_text(
    UmiApplicationLaunchAction action)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (action) {
    case UMI_APPLICATION_LAUNCH_START: return "start";
    case UMI_APPLICATION_LAUNCH_ACTIVATE: return "activate";
    case UMI_APPLICATION_LAUNCH_RESTART: return "restart";
    case UMI_APPLICATION_LAUNCH_STOP: return "stop";
    default: return "invalid";
    }
}
