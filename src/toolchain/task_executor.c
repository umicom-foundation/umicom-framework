/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/task_executor.c
 *
 * PURPOSE:
 *   Bind provider-neutral Framework build tasks to the native local Toolchain
 *   Build service used by Studio, headless automation and future applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/task_executor.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise toolchain task executor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_toolchain_task_executor_init(
    UmiToolchainTaskExecutor *executor,
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (executor == NULL || profile == NULL || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(executor, 0, sizeof(*executor));
    executor->structure_size = (uint32_t)sizeof(*executor);
    executor->api_version = UMI_TOOLCHAIN_TASK_EXECUTOR_API_VERSION;
    executor->profile = profile;
    executor->environment = environment;
    executor->request = *request;
    executor->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain task executor set workspace trust operation used by this module
 * and its client applications.
 */
UmiStatus umi_toolchain_task_executor_set_workspace_trust(
    UmiToolchainTaskExecutor *executor,
    const UmiWorkspaceTrustStore *trust_store,
    const char *workspace_root)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (executor == NULL ||
        executor->structure_size != sizeof(*executor) ||
        executor->api_version != UMI_TOOLCHAIN_TASK_EXECUTOR_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (trust_store == NULL || workspace_root == NULL ||
        workspace_root[0] == '\0') {
        executor->trust_store = NULL;
        executor->workspace_root[0] = '\0';
        executor->revision += 1U;
        return UMI_STATUS_OK;
    }

    status = copy_text(executor->workspace_root,
                       sizeof(executor->workspace_root),
                       workspace_root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    executor->trust_store = trust_store;
    executor->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain task executor authorise operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_task_executor_authorise(
    const UmiToolchainTaskExecutor *executor,
    const UmiBuildTaskSnapshot *task)
{
    UmiWorkspaceTrustRecord trust;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (executor == NULL || task == NULL ||
        executor->structure_size != sizeof(*executor) ||
        executor->api_version != UMI_TOOLCHAIN_TASK_EXECUTOR_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Create this optional product surface only when its build option is enabled. */
    if ((task->flags & UMI_BUILD_TASK_REQUIRES_WORKSPACE_TRUST) == 0U) {
        return UMI_STATUS_OK;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (executor->trust_store == NULL ||
        executor->workspace_root[0] == '\0') {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    status = umi_workspace_trust_store_get(
        executor->trust_store,
        executor->workspace_root,
        &trust);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    return trust.level == UMI_WORKSPACE_TRUSTED
        ? UMI_STATUS_OK
        : UMI_STATUS_PERMISSION_DENIED;
}

/*
 * Provide the toolchain task executor action operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_task_executor_action(
    const UmiBuildTaskSnapshot *task,
    UmiBuildAction *out_action)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (task == NULL || out_action == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Select the behaviour associated with the requested command or state value. */
    switch (task->kind) {
        case UMI_BUILD_TASK_COMMAND:
            *out_action = UMI_BUILD_COMMAND;
            return UMI_STATUS_OK;
        case UMI_BUILD_TASK_CONFIGURE:
            *out_action = UMI_BUILD_CONFIGURE;
            return UMI_STATUS_OK;
        case UMI_BUILD_TASK_BUILD:
            *out_action = UMI_BUILD_COMPILE;
            return UMI_STATUS_OK;
        case UMI_BUILD_TASK_TEST:
            *out_action = UMI_BUILD_TEST;
            return UMI_STATUS_OK;
        case UMI_BUILD_TASK_RUN:
            *out_action = UMI_BUILD_RUN;
            return UMI_STATUS_OK;
        case UMI_BUILD_TASK_CLEAN:
            *out_action = UMI_BUILD_CLEAN;
            return UMI_STATUS_OK;
        case UMI_BUILD_TASK_INSTALL:
            *out_action = UMI_BUILD_INSTALL;
            return UMI_STATUS_OK;
        case UMI_BUILD_TASK_PACKAGE:
            *out_action = UMI_BUILD_PACKAGE;
            return UMI_STATUS_OK;

        /*
         * COMMAND needs the dedicated shell-independent argv parser/provider.
         * DEPLOY belongs to the deployment provider/controller boundary.
         * COMPOSITE is expanded by the task orchestrator and is never executed
         * as a process itself. Refusing these here avoids unsafe shell parsing
         * or duplicated deployment logic.
         */
  //     case UMI_BUILD_TASK_COMMAND:
        case UMI_BUILD_TASK_DEPLOY:
        case UMI_BUILD_TASK_COMPOSITE:
            return UMI_STATUS_NOT_IMPLEMENTED;
        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }
}

/*
 * Provide the compose task environment operation used by this module and its client
 * applications.
 */
static UmiStatus compose_task_environment(
    const UmiToolchainTaskExecutor *executor,
    const UmiBuildTaskSnapshot *task,
    UmiEnvironmentPlan *out_environment)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (executor == NULL || task == NULL || out_environment == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_environment_plan_compose(
        executor->environment,
        NULL,
        out_environment);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < task->environment_count; ++index) {
        status = umi_environment_plan_set_assignment(
            out_environment,
            task->environment[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return umi_environment_plan_validate(out_environment);
}

/*
 * Perform toolchain task executor through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_toolchain_task_executor_execute(
    UmiToolchainTaskExecutor *executor,
    const UmiBuildTaskSnapshot *task,
    UmiBuildReport *out_report)
{
    UmiBuildRequest request;
    UmiBuildCommand command;
    UmiEnvironmentPlan environment;
    UmiBuildAction action;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (executor == NULL || task == NULL ||
        executor->structure_size != sizeof(*executor) ||
        executor->api_version != UMI_TOOLCHAIN_TASK_EXECUTOR_API_VERSION ||
        executor->profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_toolchain_task_executor_authorise(executor, task);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_toolchain_task_executor_action(task, &action);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = compose_task_environment(executor, task, &environment);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    request = executor->request;

    /* Apply this branch only when its contract condition is satisfied. */
    if (task->working_directory[0] != '\0') {
        request.source_root = task->working_directory;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (task->timeout_ms > 0U) {
        request.timeout_ms = task->timeout_ms;
    }

    /* Create this optional product surface only when its build option is enabled. */
    if (task->kind == UMI_BUILD_TASK_COMMAND) {
        char message[256];

        status = umi_build_command_parse(
            task->command,
            &command,
            message,
            sizeof(message));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        request.executable = command.program;
        request.run_arguments = command.arguments;
        request.run_argument_count = command.argument_count;

        /* Use the shared build helper when it is available from the parent composition. */
        if (command.working_directory[0] != '\0') {
            request.source_root = command.working_directory;
        }
    }

    status = umi_build_execute(
        executor->profile,
        &environment,
        action,
        &request,
        out_report);

    executor->revision += 1U;
    return status;
}
