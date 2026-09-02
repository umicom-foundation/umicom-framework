/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/task_executor.h
 *
 * PURPOSE:
 *   Execute semantic Framework build tasks through the validated native
 *   Toolchain Build service without moving process ownership into applications.
 *
 * ARCHITECTURE:
 *   - Build/task orchestration stays provider-neutral in Umicom::build.
 *   - Native local execution lives here.
 *   - Command tasks are parsed into owned argv; no shell is involved.
 *   - Task environment overlays are bounded and isolated.
 *   - Trust-required tasks are blocked unless the workspace is explicitly
 *     trusted by Framework security state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_TASK_EXECUTOR_H
#define UMICOM_TOOLCHAIN_TASK_EXECUTOR_H

#include <stdint.h>

#include "umicom/build/task.h"
#include "umicom/security/workspace_trust.h"
#include "umicom/toolchain/build.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_TASK_EXECUTOR_API_VERSION 2U

/**
 * Represent the toolchain task executor data shared with callers of this public contract.
 */
typedef struct UmiToolchainTaskExecutor {
    uint32_t structure_size;
    uint32_t api_version;
    const UmiToolchainProfile *profile;
    UmiEnvironmentPlan *environment;
    UmiBuildRequest request;
    const UmiWorkspaceTrustStore *trust_store;
    char workspace_root[UMI_PATH_CAPACITY];
    uint64_t revision;
} UmiToolchainTaskExecutor;

/**
 * Initialise toolchain task executor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_toolchain_task_executor_init(
    UmiToolchainTaskExecutor *executor,
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *request);

/**
 * Provide the toolchain task executor set workspace trust operation used by this module
 * and its client applications.
 */
UmiStatus umi_toolchain_task_executor_set_workspace_trust(
    UmiToolchainTaskExecutor *executor,
    const UmiWorkspaceTrustStore *trust_store,
    const char *workspace_root);

/**
 * Provide the toolchain task executor authorise operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_task_executor_authorise(
    const UmiToolchainTaskExecutor *executor,
    const UmiBuildTaskSnapshot *task);

/**
 * Provide the toolchain task executor action operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_task_executor_action(
    const UmiBuildTaskSnapshot *task,
    UmiBuildAction *out_action);

/**
 * Perform toolchain task executor through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_toolchain_task_executor_execute(
    UmiToolchainTaskExecutor *executor,
    const UmiBuildTaskSnapshot *task,
    UmiBuildReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
