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
/**
 * Represent the terminal remote terminal command history data shared with callers of this
 * public contract.
 */
typedef struct UmiTerminalRemoteTerminalCommandHistory { UmiTerminalRemoteNamedEntry items[UMI_TERMINAL_REMOTE_CATALOGUE_CAPACITY]; size_t count; uint64_t revision; } UmiTerminalRemoteTerminalCommandHistory;
/**
 * Initialise terminal remote terminal command history from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_terminal_command_history_init(UmiTerminalRemoteTerminalCommandHistory *value);
/**
 * Add terminal remote terminal command history only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_terminal_remote_terminal_command_history_add(UmiTerminalRemoteTerminalCommandHistory *value, const char *id, const char *label);
/**
 * Find terminal remote terminal command history while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiTerminalRemoteNamedEntry *umi_terminal_remote_terminal_command_history_find(const UmiTerminalRemoteTerminalCommandHistory *value, const char *id);
/**
 * Return the number of records represented by terminal remote terminal command history
 * enabled without changing their state.
 */
size_t umi_terminal_remote_terminal_command_history_enabled_count(const UmiTerminalRemoteTerminalCommandHistory *value);
#ifdef __cplusplus
}
#endif
#endif
