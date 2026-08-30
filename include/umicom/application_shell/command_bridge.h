/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/command_bridge.h
 *
 * PURPOSE:
 *   Connect reusable shell contributions to the authoritative Framework command
 *   registry so visible menus and toolbar actions execute real commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_COMMAND_BRIDGE_H
#define UMICOM_APPLICATION_SHELL_COMMAND_BRIDGE_H

#include "umicom/application_shell/registry.h"
#include "umicom/runtime/command_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_application_shell_command_bridge_reconcile(
    UmiApplicationShellRegistry *shell,
    const UmiCommandRegistry *commands);

UmiStatus umi_application_shell_command_bridge_execute(
    UmiApplicationShellRegistry *shell,
    UmiCommandRegistry *commands,
    const char *contribution_id,
    const char *argument,
    char *out_message,
    size_t message_capacity);

#ifdef __cplusplus
}
#endif

#endif
