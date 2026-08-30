/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_command_history.h
 *
 * PURPOSE:
 *   Manage bounded terminal command history entries with deterministic lookup and duplicate rejection.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_COMMAND_HISTORY_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_COMMAND_HISTORY_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteTerminalCommandHistory { UmiTerminalRemoteNamedEntry items[UMI_TERMINAL_REMOTE_CATALOGUE_CAPACITY]; size_t count; uint64_t revision; } UmiTerminalRemoteTerminalCommandHistory;
void umi_terminal_remote_terminal_command_history_init(UmiTerminalRemoteTerminalCommandHistory *value);
UmiStatus umi_terminal_remote_terminal_command_history_add(UmiTerminalRemoteTerminalCommandHistory *value, const char *id, const char *label);
const UmiTerminalRemoteNamedEntry *umi_terminal_remote_terminal_command_history_find(const UmiTerminalRemoteTerminalCommandHistory *value, const char *id);
size_t umi_terminal_remote_terminal_command_history_enabled_count(const UmiTerminalRemoteTerminalCommandHistory *value);
#ifdef __cplusplus
}
#endif
#endif
