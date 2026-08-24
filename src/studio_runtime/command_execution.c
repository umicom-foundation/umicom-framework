/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_execution.c
 *
 * PURPOSE:
 *   Implement execute-then-activate semantics for Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_execution.h"

UmiStatus umi_studio_runtime_execute_command(
    UmiStudioRuntimeBindings *bindings,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiStatus status;

    if (bindings == NULL || command_id == NULL ||
        command_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_bindings_validate(bindings);
    if (status != UMI_STATUS_OK) return status;

    status = umi_command_registry_execute(
        bindings->commands,
        command_id,
        argument,
        out_message,
        message_capacity);
    if (status != UMI_STATUS_OK) return status;

    return umi_studio_surface_activate_for_command(
        bindings,
        command_id);
}

UmiStatus umi_studio_runtime_execute_contribution(
    UmiStudioRuntimeBindings *bindings,
    const char *contribution_id,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiApplicationShellContribution contribution;
    UmiStatus status;

    if (bindings == NULL || contribution_id == NULL ||
        contribution_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_bindings_validate(bindings);
    if (status != UMI_STATUS_OK) return status;

    status = umi_application_shell_registry_find(
        bindings->shell_registry,
        contribution_id,
        &contribution);
    if (status != UMI_STATUS_OK) return status;

    status = umi_application_shell_command_bridge_execute(
        bindings->shell_registry,
        bindings->commands,
        contribution_id,
        argument,
        out_message,
        message_capacity);
    if (status != UMI_STATUS_OK) return status;

    if (contribution.command_id[0] != '\0') {
        status = umi_studio_surface_activate_for_command(
            bindings,
            contribution.command_id);
    }

    return status;
}
