/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_command_policy.h
 *
 * PURPOSE:
 *   Evaluate trust, write and interactive gates for terminal command policy.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_COMMAND_POLICY_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_COMMAND_POLICY_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote terminal command policy data shared with callers of this
 * public contract.
 */
typedef struct UmiTerminalRemoteTerminalCommandPolicy { bool require_trust; bool allow_write; bool allow_interactive; } UmiTerminalRemoteTerminalCommandPolicy;
/**
 * Initialise terminal remote terminal command policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_terminal_command_policy_init(UmiTerminalRemoteTerminalCommandPolicy *value);
/**
 * Provide the terminal remote terminal command policy allows operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_terminal_command_policy_allows(const UmiTerminalRemoteTerminalCommandPolicy *value,bool trusted,bool write_request,bool interactive_request);
#ifdef __cplusplus
}
#endif
#endif
