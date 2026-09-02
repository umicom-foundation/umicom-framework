/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_connection.h
 *
 * PURPOSE:
 *   Model lifecycle state for remote connection with guarded transitions.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_CONNECTION_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_CONNECTION_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote remote connection data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteRemoteConnection { char id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; UmiTerminalRemoteState state; uint64_t revision; } UmiTerminalRemoteRemoteConnection;
/**
 * Initialise terminal remote remote connection from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_connection_init(UmiTerminalRemoteRemoteConnection *value,const char *id);
/**
 * Provide the terminal remote remote connection transition operation used by this module
 * and its client applications.
 */
UmiStatus umi_terminal_remote_remote_connection_transition(UmiTerminalRemoteRemoteConnection *value,UmiTerminalRemoteState next);
/**
 * Provide the terminal remote remote connection usable operation used by this module and
 * its client applications.
 */
bool umi_terminal_remote_remote_connection_usable(const UmiTerminalRemoteRemoteConnection *value);
#ifdef __cplusplus
}
#endif
#endif
