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
typedef struct UmiTerminalRemoteTerminalSession { char id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; UmiTerminalRemoteState state; uint64_t revision; } UmiTerminalRemoteTerminalSession;
void umi_terminal_remote_terminal_session_init(UmiTerminalRemoteTerminalSession *value,const char *id);
UmiStatus umi_terminal_remote_terminal_session_transition(UmiTerminalRemoteTerminalSession *value,UmiTerminalRemoteState next);
bool umi_terminal_remote_terminal_session_usable(const UmiTerminalRemoteTerminalSession *value);
#ifdef __cplusplus
}
#endif
#endif
