/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/task_executor.h
 *
 * PURPOSE:
 *   Execute semantic Framework build tasks through the validated Toolchain
 *   Build service without moving process ownership into Umicom Studio.
 *
 * ARCHITECTURE:
 *   Build task/orchestration remains provider-neutral in Umicom::build.
 *   This adapter is the native local-process provider. Remote/container
 *   providers can implement the same semantic task kinds independently.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_TASK_EXECUTOR_H
#define UMICOM_TOOLCHAIN_TASK_EXECUTOR_H

#include <stdint.h>

#include "umicom/build/task.h"
#include "umicom/toolchain/build.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_TASK_EXECUTOR_API_VERSION 1U

typedef struct UmiToolchainTaskExecutor {
    uint32_t structure_size;
    uint32_t api_version;
    const UmiToolchainProfile *profile;
    UmiEnvironmentPlan *environment;
    UmiBuildRequest request;
    uint64_t revision;
} UmiToolchainTaskExecutor;

UmiStatus umi_toolchain_task_executor_init(
    UmiToolchainTaskExecutor *executor,
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *request);

UmiStatus umi_toolchain_task_executor_action(
    const UmiBuildTaskSnapshot *task,
    UmiBuildAction *out_action);

UmiStatus umi_toolchain_task_executor_execute(
    UmiToolchainTaskExecutor *executor,
    const UmiBuildTaskSnapshot *task,
    UmiBuildReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
