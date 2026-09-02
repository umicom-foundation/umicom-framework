/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_connection_catalogue.h
 *
 * PURPOSE:
 *   Manage bounded remote connection catalogue entries with deterministic lookup and duplicate rejection.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_CONNECTION_CATALOGUE_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_CONNECTION_CATALOGUE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote remote connection catalogue data shared with callers of
 * this public contract.
 */
typedef struct UmiTerminalRemoteRemoteConnectionCatalogue { UmiTerminalRemoteNamedEntry items[UMI_TERMINAL_REMOTE_CATALOGUE_CAPACITY]; size_t count; uint64_t revision; } UmiTerminalRemoteRemoteConnectionCatalogue;
/**
 * Initialise terminal remote remote connection catalogue from caller-provided values so
 * later operations receive a known state.
 */
void umi_terminal_remote_remote_connection_catalogue_init(UmiTerminalRemoteRemoteConnectionCatalogue *value);
/**
 * Add terminal remote remote connection catalogue only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_terminal_remote_remote_connection_catalogue_add(UmiTerminalRemoteRemoteConnectionCatalogue *value, const char *id, const char *label);
/**
 * Find terminal remote remote connection catalogue while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiTerminalRemoteNamedEntry *umi_terminal_remote_remote_connection_catalogue_find(const UmiTerminalRemoteRemoteConnectionCatalogue *value, const char *id);
/**
 * Return the number of records represented by terminal remote remote connection catalogue
 * enabled without changing their state.
 */
size_t umi_terminal_remote_remote_connection_catalogue_enabled_count(const UmiTerminalRemoteRemoteConnectionCatalogue *value);
#ifdef __cplusplus
}
#endif
#endif
