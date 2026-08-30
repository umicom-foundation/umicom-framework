/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_session_catalogue.h
 *
 * PURPOSE:
 *   Manage bounded terminal session catalogue entries with deterministic lookup and duplicate rejection.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_SESSION_CATALOGUE_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_SESSION_CATALOGUE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteTerminalSessionCatalogue { UmiTerminalRemoteNamedEntry items[UMI_TERMINAL_REMOTE_CATALOGUE_CAPACITY]; size_t count; uint64_t revision; } UmiTerminalRemoteTerminalSessionCatalogue;
void umi_terminal_remote_terminal_session_catalogue_init(UmiTerminalRemoteTerminalSessionCatalogue *value);
UmiStatus umi_terminal_remote_terminal_session_catalogue_add(UmiTerminalRemoteTerminalSessionCatalogue *value, const char *id, const char *label);
const UmiTerminalRemoteNamedEntry *umi_terminal_remote_terminal_session_catalogue_find(const UmiTerminalRemoteTerminalSessionCatalogue *value, const char *id);
size_t umi_terminal_remote_terminal_session_catalogue_enabled_count(const UmiTerminalRemoteTerminalSessionCatalogue *value);
#ifdef __cplusplus
}
#endif
#endif
