/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/shell_catalogue.h
 *
 * PURPOSE:
 *   Manage bounded shell catalogue entries with deterministic lookup and duplicate rejection.
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
#ifndef UMICOM_TERMINAL_REMOTE_SHELL_CATALOGUE_H
#define UMICOM_TERMINAL_REMOTE_SHELL_CATALOGUE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote shell catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteShellCatalogue { UmiTerminalRemoteNamedEntry items[UMI_TERMINAL_REMOTE_CATALOGUE_CAPACITY]; size_t count; uint64_t revision; } UmiTerminalRemoteShellCatalogue;
/**
 * Initialise terminal remote shell catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_shell_catalogue_init(UmiTerminalRemoteShellCatalogue *value);
/**
 * Add terminal remote shell catalogue only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_terminal_remote_shell_catalogue_add(UmiTerminalRemoteShellCatalogue *value, const char *id, const char *label);
/**
 * Find terminal remote shell catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiTerminalRemoteNamedEntry *umi_terminal_remote_shell_catalogue_find(const UmiTerminalRemoteShellCatalogue *value, const char *id);
/**
 * Return the number of records represented by terminal remote shell catalogue enabled
 * without changing their state.
 */
size_t umi_terminal_remote_shell_catalogue_enabled_count(const UmiTerminalRemoteShellCatalogue *value);
#ifdef __cplusplus
}
#endif
#endif
