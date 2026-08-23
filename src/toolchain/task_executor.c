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

UmiStatus umi_toolchain_task_executor_action(
    const UmiBuildTaskSnapshot *task,
    UmiBuildAction *out_action)
{
    if (task == NULL || out_action == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    switch (task->kind) {
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

UmiStatus umi_toolchain_task_executor_execute(
    UmiToolchainTaskExecutor *executor,
    const UmiBuildTaskSnapshot *task,
    UmiBuildReport *out_report)
{
    UmiBuildRequest request;
    UmiBuildAction action;
    UmiStatus status;

    if (executor == NULL || task == NULL ||
        executor->structure_size != sizeof(*executor) ||
        executor->api_version != UMI_TOOLCHAIN_TASK_EXECUTOR_API_VERSION ||
        executor->profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (task->environment_count != 0U) {
        /*
         * Environment overlays are intentionally not ignored. A later provider
         * slice will translate the task's bounded NAME=VALUE entries into an
         * isolated child environment before execution.
         */
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    status = umi_toolchain_task_executor_action(task, &action);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    request = executor->request;

    if (task->working_directory[0] != '\0') {
        request.source_root = task->working_directory;
    }
    if (task->timeout_ms > 0U) {
        request.timeout_ms = task->timeout_ms;
    }

    status = umi_build_execute(
        executor->profile,
        executor->environment,
        action,
        &request,
        out_report);

    executor->revision += 1U;
    return status;
}
