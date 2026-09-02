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
/**
 * Represent the terminal remote terminal session catalogue data shared with callers of
 * this public contract.
 */
typedef struct UmiTerminalRemoteTerminalSessionCatalogue { UmiTerminalRemoteNamedEntry items[UMI_TERMINAL_REMOTE_CATALOGUE_CAPACITY]; size_t count; uint64_t revision; } UmiTerminalRemoteTerminalSessionCatalogue;
/**
 * Initialise terminal remote terminal session catalogue from caller-provided values so
 * later operations receive a known state.
 */
void umi_terminal_remote_terminal_session_catalogue_init(UmiTerminalRemoteTerminalSessionCatalogue *value);
/**
 * Add terminal remote terminal session catalogue only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_terminal_remote_terminal_session_catalogue_add(UmiTerminalRemoteTerminalSessionCatalogue *value, const char *id, const char *label);
/**
 * Find terminal remote terminal session catalogue while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiTerminalRemoteNamedEntry *umi_terminal_remote_terminal_session_catalogue_find(const UmiTerminalRemoteTerminalSessionCatalogue *value, const char *id);
/**
 * Return the number of records represented by terminal remote terminal session catalogue
 * enabled without changing their state.
 */
size_t umi_terminal_remote_terminal_session_catalogue_enabled_count(const UmiTerminalRemoteTerminalSessionCatalogue *value);
#ifdef __cplusplus
}
#endif
#endif
