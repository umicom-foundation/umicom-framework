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

typedef struct UmiDeveloperWorkbenchCommandService
    UmiDeveloperWorkbenchCommandService;

UmiStatus umi_developer_workbench_command_service_create(
    UmiCommandRegistry *commands,
    UmiDeveloperWorkbenchLifecycle *lifecycle,
    UmiDeveloperWorkbenchConfigurationRegistry *configurations,
    const UmiToolchainProfile *profile,
    UmiDeveloperWorkbenchActionRegistry *actions,
    UmiDeveloperWorkbenchCommandService **out_service);

void umi_developer_workbench_command_service_destroy(
    UmiDeveloperWorkbenchCommandService *service);

UmiStatus umi_developer_workbench_command_service_register_all(
    UmiDeveloperWorkbenchCommandService *service);

UmiStatus umi_developer_workbench_command_service_set_workspace_trust(
    UmiDeveloperWorkbenchCommandService *service,
    const UmiWorkspaceTrustStore *trust_store);

int umi_developer_workbench_command_service_is_enabled(
    UmiDeveloperWorkbenchCommandService *service,
    const char *command_id,
    const char *argument);

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
