/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_session.h
 *
 * PURPOSE:
 *   Model lifecycle state for terminal session with guarded transitions.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_SESSION_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_SESSION_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote terminal session data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteTerminalSession { char id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; UmiTerminalRemoteState state; uint64_t revision; } UmiTerminalRemoteTerminalSession;
/**
 * Initialise terminal remote terminal session from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_terminal_session_init(UmiTerminalRemoteTerminalSession *value,const char *id);
/**
 * Provide the terminal remote terminal session transition operation used by this module
 * and its client applications.
 */
UmiStatus umi_terminal_remote_terminal_session_transition(UmiTerminalRemoteTerminalSession *value,UmiTerminalRemoteState next);
/**
 * Provide the terminal remote terminal session usable operation used by this module and
 * its client applications.
 */
bool umi_terminal_remote_terminal_session_usable(const UmiTerminalRemoteTerminalSession *value);
#ifdef __cplusplus
}
#endif
#endif
