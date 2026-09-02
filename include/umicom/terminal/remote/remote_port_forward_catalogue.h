/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_port_forward_catalogue.h
 *
 * PURPOSE:
 *   Manage bounded remote port forward catalogue entries with deterministic lookup and duplicate rejection.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_PORT_FORWARD_CATALOGUE_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_PORT_FORWARD_CATALOGUE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote remote port forward catalogue data shared with callers of
 * this public contract.
 */
typedef struct UmiTerminalRemoteRemotePortForwardCatalogue { UmiTerminalRemoteNamedEntry items[UMI_TERMINAL_REMOTE_CATALOGUE_CAPACITY]; size_t count; uint64_t revision; } UmiTerminalRemoteRemotePortForwardCatalogue;
/**
 * Initialise terminal remote remote port forward catalogue from caller-provided values so
 * later operations receive a known state.
 */
void umi_terminal_remote_remote_port_forward_catalogue_init(UmiTerminalRemoteRemotePortForwardCatalogue *value);
/**
 * Add terminal remote remote port forward catalogue only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_terminal_remote_remote_port_forward_catalogue_add(UmiTerminalRemoteRemotePortForwardCatalogue *value, const char *id, const char *label);
/**
 * Find terminal remote remote port forward catalogue while leaving the underlying
 * catalogue or model owned by this module.
 */
const UmiTerminalRemoteNamedEntry *umi_terminal_remote_remote_port_forward_catalogue_find(const UmiTerminalRemoteRemotePortForwardCatalogue *value, const char *id);
/**
 * Return the number of records represented by terminal remote remote port forward
 * catalogue enabled without changing their state.
 */
size_t umi_terminal_remote_remote_port_forward_catalogue_enabled_count(const UmiTerminalRemoteRemotePortForwardCatalogue *value);
#ifdef __cplusplus
}
#endif
#endif
