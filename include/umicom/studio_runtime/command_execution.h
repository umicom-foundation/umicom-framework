/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_execution.h
 *
 * PURPOSE:
 *   Execute real Application Shell/runtime commands and then apply the
 *   presentation activation associated with a successful command.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_COMMAND_EXECUTION_H
#define UMICOM_STUDIO_RUNTIME_COMMAND_EXECUTION_H

#include "umicom/application_shell/command_bridge.h"
#include "umicom/studio_runtime/surface_activator.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_runtime_execute_command(
    UmiStudioRuntimeBindings *bindings,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity);

UmiStatus umi_studio_runtime_execute_contribution(
    UmiStudioRuntimeBindings *bindings,
    const char *contribution_id,
    const char *argument,
    char *out_message,
    size_t message_capacity);

#ifdef __cplusplus
}
#endif
#endif
