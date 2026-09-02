/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/command_service.h
 *
 * PURPOSE:
 *   Register professional developer commands in the canonical Framework command
 *   registry and route them to real lifecycle operations or explicitly bound
 *   product actions.
 *
 * ARCHITECTURE:
 *   A command is never advertised as enabled merely because its text exists.
 *   Native lifecycle commands require a ready toolchain/configuration. Product
 *   actions require an explicit binding. Trust-required commands are disabled
 *   unless Framework workspace trust says the workspace is trusted.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_COMMAND_SERVICE_H
#define UMICOM_DEVELOPER_WORKBENCH_COMMAND_SERVICE_H

#include "umicom/developer_workbench/action_registry.h"
#include "umicom/developer_workbench/lifecycle.h"
#include "umicom/runtime/command_registry.h"
#include "umicom/security/workspace_trust.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer workbench command service data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperWorkbenchCommandService
    UmiDeveloperWorkbenchCommandService;

/**
 * Initialise developer workbench command service from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_workbench_command_service_create(
    UmiCommandRegistry *commands,
    UmiDeveloperWorkbenchLifecycle *lifecycle,
    UmiDeveloperWorkbenchConfigurationRegistry *configurations,
    const UmiToolchainProfile *profile,
    UmiDeveloperWorkbenchActionRegistry *actions,
    UmiDeveloperWorkbenchCommandService **out_service);

/**
 * Release or reset state held by developer workbench command service so the same storage
 * can be reused safely.
 */
void umi_developer_workbench_command_service_destroy(
    UmiDeveloperWorkbenchCommandService *service);

/**
 * Provide the developer workbench command service register all operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_command_service_register_all(
    UmiDeveloperWorkbenchCommandService *service);

/**
 * Provide the developer workbench command service set workspace trust operation used by
 * this module and its client applications.
 */
UmiStatus umi_developer_workbench_command_service_set_workspace_trust(
    UmiDeveloperWorkbenchCommandService *service,
    const UmiWorkspaceTrustStore *trust_store);

/**
 * Provide the developer workbench command service is enabled operation used by this module
 * and its client applications.
 */
int umi_developer_workbench_command_service_is_enabled(
    UmiDeveloperWorkbenchCommandService *service,
    const char *command_id,
    const char *argument);

/**
 * Perform developer workbench command service through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_developer_workbench_command_service_execute(
    UmiDeveloperWorkbenchCommandService *service,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity);

#ifdef __cplusplus
}
#endif

#endif
