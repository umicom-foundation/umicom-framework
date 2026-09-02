/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/lifecycle.c
 *
 * PURPOSE:
 *   Implement real native developer lifecycle execution behind workbench
 *   commands while preserving Framework cancellation and execution evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/lifecycle.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

struct UmiDeveloperWorkbenchLifecycle {
    const UmiToolchainProfile *profile;
    UmiEnvironmentPlan *environment;
    UmiDeveloperWorkbenchConfigurationRegistry *configurations;
    UmiDeveloperWorkbenchOperationHistory *history;
    UmiCancellationToken *cancellation;
    _Atomic int running;
};

/*
 * Initialise developer workbench lifecycle from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_workbench_lifecycle_create(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    UmiDeveloperWorkbenchConfigurationRegistry *configurations,
    UmiDeveloperWorkbenchOperationHistory *history,
    UmiDeveloperWorkbenchLifecycle **out_lifecycle)
{
    UmiDeveloperWorkbenchLifecycle *lifecycle;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || configurations == NULL ||
        history == NULL || out_lifecycle == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_lifecycle = NULL;
    lifecycle = (UmiDeveloperWorkbenchLifecycle *)calloc(
        1U, sizeof(*lifecycle));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lifecycle == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    lifecycle->profile = profile;
    lifecycle->environment = environment;
    lifecycle->configurations = configurations;
    lifecycle->history = history;

    status = umi_cancellation_token_create(&lifecycle->cancellation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(lifecycle);
        return status;
    }

    *out_lifecycle = lifecycle;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer workbench lifecycle so the same storage can be
 * reused safely.
 */
void umi_developer_workbench_lifecycle_destroy(
    UmiDeveloperWorkbenchLifecycle *lifecycle)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lifecycle == NULL) return;
    umi_cancellation_token_destroy(lifecycle->cancellation);
    free(lifecycle);
}

/*
 * Provide the execute build action operation used by this module and its client
 * applications.
 */
static UmiStatus execute_build_action(
    UmiDeveloperWorkbenchLifecycle *lifecycle,
    const UmiDeveloperWorkbenchConfiguration *configuration,
    const char *command_id,
    UmiBuildAction action,
    int clean,
    UmiBuildReport *out_report)
{
    UmiBuildRequest request;
    UmiBuildReport local_report;
    UmiBuildReport *report =
        out_report != NULL ? out_report : &local_report;
    UmiStatus status;

    status = umi_developer_workbench_configuration_to_build_request(
        configuration, &request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_cancellation_token_reset(lifecycle->cancellation);
    request.cancellation = lifecycle->cancellation;
    request.clean = clean;

    atomic_store_explicit(
        &lifecycle->running, 1, memory_order_release);
    status = umi_build_execute(
        lifecycle->profile,
        lifecycle->environment,
        action,
        &request,
        report);
    atomic_store_explicit(
        &lifecycle->running, 0, memory_order_release);

    (void)umi_developer_workbench_operation_history_append(
        lifecycle->history,
        command_id,
        action,
        status,
        report);
    return status;
}

/*
 * Perform developer workbench lifecycle through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_developer_workbench_lifecycle_execute(
    UmiDeveloperWorkbenchLifecycle *lifecycle,
    const char *command_id,
    UmiDeveloperWorkbenchCommandAction action,
    UmiBuildReport *out_report)
{
    UmiDeveloperWorkbenchConfiguration configuration;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lifecycle == NULL || command_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (action == UMI_DEVELOPER_WORKBENCH_ACTION_CANCEL) {
        umi_developer_workbench_lifecycle_cancel(lifecycle);
        return UMI_STATUS_OK;
    }

    status = umi_developer_workbench_configuration_registry_active(
        lifecycle->configurations, &configuration);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Select the behaviour associated with the requested command or state value. */
    switch (action) {
        case UMI_DEVELOPER_WORKBENCH_ACTION_CONFIGURE:
            return execute_build_action(
                lifecycle, &configuration, command_id,
                UMI_BUILD_CONFIGURE, 0, out_report);

        case UMI_DEVELOPER_WORKBENCH_ACTION_BUILD:
            return execute_build_action(
                lifecycle, &configuration, command_id,
                UMI_BUILD_COMPILE, 0, out_report);

        case UMI_DEVELOPER_WORKBENCH_ACTION_REBUILD: {
            UmiBuildReport clean_report;

            status = execute_build_action(
                lifecycle, &configuration, command_id,
                UMI_BUILD_CLEAN, 0, &clean_report);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;

            status = execute_build_action(
                lifecycle, &configuration, command_id,
                UMI_BUILD_CONFIGURE, 0, out_report);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;

            return execute_build_action(
                lifecycle, &configuration, command_id,
                UMI_BUILD_COMPILE, 0, out_report);
        }

        case UMI_DEVELOPER_WORKBENCH_ACTION_CLEAN:
            return execute_build_action(
                lifecycle, &configuration, command_id,
                UMI_BUILD_CLEAN, 0, out_report);

        case UMI_DEVELOPER_WORKBENCH_ACTION_TEST:
            return execute_build_action(
                lifecycle, &configuration, command_id,
                UMI_BUILD_TEST, 0, out_report);

        case UMI_DEVELOPER_WORKBENCH_ACTION_RUN:
            return execute_build_action(
                lifecycle, &configuration, command_id,
                UMI_BUILD_RUN, 0, out_report);

        case UMI_DEVELOPER_WORKBENCH_ACTION_INSTALL:
            return execute_build_action(
                lifecycle, &configuration, command_id,
                UMI_BUILD_INSTALL, 0, out_report);

        case UMI_DEVELOPER_WORKBENCH_ACTION_PACKAGE:
            return execute_build_action(
                lifecycle, &configuration, command_id,
                UMI_BUILD_PACKAGE, 0, out_report);

        case UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL:
        case UMI_DEVELOPER_WORKBENCH_ACTION_CANCEL:
        default:
            return UMI_STATUS_NOT_IMPLEMENTED;
    }
}

/*
 * Provide the developer workbench lifecycle cancel operation used by this module and its
 * client applications.
 */
void umi_developer_workbench_lifecycle_cancel(
    UmiDeveloperWorkbenchLifecycle *lifecycle)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lifecycle != NULL) {
        umi_cancellation_token_request(lifecycle->cancellation);
    }
}

/*
 * Provide the developer workbench lifecycle is cancel requested operation used by this
 * module and its client applications.
 */
int umi_developer_workbench_lifecycle_is_cancel_requested(
    const UmiDeveloperWorkbenchLifecycle *lifecycle)
{
    return lifecycle != NULL &&
        umi_cancellation_token_is_requested(lifecycle->cancellation);
}

/*
 * Provide the developer workbench lifecycle is running operation used by this module and
 * its client applications.
 */
int umi_developer_workbench_lifecycle_is_running(
    const UmiDeveloperWorkbenchLifecycle *lifecycle)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lifecycle == NULL) return 0;

    return atomic_load_explicit(
        &lifecycle->running,
        memory_order_acquire);
}
