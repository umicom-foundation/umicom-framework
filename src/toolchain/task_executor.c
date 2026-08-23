/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/task_executor.c
 *
 * PURPOSE:
 *   Bind provider-neutral Framework build tasks to the native local Toolchain
 *   Build service used by Studio, headless automation and future applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/task_executor.h"

#include <string.h>

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_toolchain_task_executor_init(
    UmiToolchainTaskExecutor *executor,
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *request)
{
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

UmiStatus umi_toolchain_task_executor_set_workspace_trust(
    UmiToolchainTaskExecutor *executor,
    const UmiWorkspaceTrustStore *trust_store,
    const char *workspace_root)
{
    UmiStatus status;

    if (executor == NULL ||
        executor->structure_size != sizeof(*executor) ||
        executor->api_version != UMI_TOOLCHAIN_TASK_EXECUTOR_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

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
    if (status != UMI_STATUS_OK) return status;

    executor->trust_store = trust_store;
    executor->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_toolchain_task_executor_authorise(
    const UmiToolchainTaskExecutor *executor,
    const UmiBuildTaskSnapshot *task)
{
    UmiWorkspaceTrustRecord trust;
    UmiStatus status;

    if (executor == NULL || task == NULL ||
        executor->structure_size != sizeof(*executor) ||
        executor->api_version != UMI_TOOLCHAIN_TASK_EXECUTOR_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if ((task->flags & UMI_BUILD_TASK_REQUIRES_WORKSPACE_TRUST) == 0U) {
        return UMI_STATUS_OK;
    }

    if (executor->trust_store == NULL ||
        executor->workspace_root[0] == '\0') {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    status = umi_workspace_trust_store_get(
        executor->trust_store,
        executor->workspace_root,
        &trust);
    if (status != UMI_STATUS_OK) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    return trust.level == UMI_WORKSPACE_TRUSTED
        ? UMI_STATUS_OK
        : UMI_STATUS_PERMISSION_DENIED;
}

UmiStatus umi_toolchain_task_executor_action(
    const UmiBuildTaskSnapshot *task,
    UmiBuildAction *out_action)
{
    if (task == NULL || out_action == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

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
        case UMI_BUILD_TASK_COMMAND:
        case UMI_BUILD_TASK_DEPLOY:
        case UMI_BUILD_TASK_COMPOSITE:
            return UMI_STATUS_NOT_IMPLEMENTED;
        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }
}

static UmiStatus compose_task_environment(
    const UmiToolchainTaskExecutor *executor,
    const UmiBuildTaskSnapshot *task,
    UmiEnvironmentPlan *out_environment)
{
    size_t index;
    UmiStatus status;

    if (executor == NULL || task == NULL || out_environment == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_environment_plan_compose(
        executor->environment,
        NULL,
        out_environment);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < task->environment_count; ++index) {
        status = umi_environment_plan_set_assignment(
            out_environment,
            task->environment[index]);
        if (status != UMI_STATUS_OK) return status;
    }

    return umi_environment_plan_validate(out_environment);
}

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

    if (executor == NULL || task == NULL ||
        executor->structure_size != sizeof(*executor) ||
        executor->api_version != UMI_TOOLCHAIN_TASK_EXECUTOR_API_VERSION ||
        executor->profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_toolchain_task_executor_authorise(executor, task);
    if (status != UMI_STATUS_OK) return status;

    status = umi_toolchain_task_executor_action(task, &action);
    if (status != UMI_STATUS_OK) return status;

    status = compose_task_environment(executor, task, &environment);
    if (status != UMI_STATUS_OK) return status;

    request = executor->request;

    if (task->working_directory[0] != '\0') {
        request.source_root = task->working_directory;
    }

    if (task->timeout_ms > 0U) {
        request.timeout_ms = task->timeout_ms;
    }

    if (task->kind == UMI_BUILD_TASK_COMMAND) {
        char message[256];

        status = umi_build_command_parse(
            task->command,
            &command,
            message,
            sizeof(message));
        if (status != UMI_STATUS_OK) return status;

        request.executable = command.program;
        request.run_arguments = command.arguments;
        request.run_argument_count = command.argument_count;

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
