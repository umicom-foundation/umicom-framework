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
typedef struct UmiTerminalRemoteShellCatalogue { UmiTerminalRemoteNamedEntry items[UMI_TERMINAL_REMOTE_CATALOGUE_CAPACITY]; size_t count; uint64_t revision; } UmiTerminalRemoteShellCatalogue;
void umi_terminal_remote_shell_catalogue_init(UmiTerminalRemoteShellCatalogue *value);
UmiStatus umi_terminal_remote_shell_catalogue_add(UmiTerminalRemoteShellCatalogue *value, const char *id, const char *label);
const UmiTerminalRemoteNamedEntry *umi_terminal_remote_shell_catalogue_find(const UmiTerminalRemoteShellCatalogue *value, const char *id);
size_t umi_terminal_remote_shell_catalogue_enabled_count(const UmiTerminalRemoteShellCatalogue *value);
#ifdef __cplusplus
}
#endif
#endif
