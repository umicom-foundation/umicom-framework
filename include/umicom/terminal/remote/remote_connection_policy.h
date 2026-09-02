/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_connection_policy.h
 *
 * PURPOSE:
 *   Evaluate trust, write and interactive gates for remote connection policy.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_CONNECTION_POLICY_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_CONNECTION_POLICY_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote remote connection policy data shared with callers of this
 * public contract.
 */
typedef struct UmiTerminalRemoteRemoteConnectionPolicy { bool require_trust; bool allow_write; bool allow_interactive; } UmiTerminalRemoteRemoteConnectionPolicy;
/**
 * Initialise terminal remote remote connection policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_connection_policy_init(UmiTerminalRemoteRemoteConnectionPolicy *value);
/**
 * Provide the terminal remote remote connection policy allows operation used by this
 * module and its client applications.
 */
bool umi_terminal_remote_remote_connection_policy_allows(const UmiTerminalRemoteRemoteConnectionPolicy *value,bool trusted,bool write_request,bool interactive_request);
#ifdef __cplusplus
}
#endif
#endif
