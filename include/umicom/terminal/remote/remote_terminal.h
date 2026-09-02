/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_terminal.h
 *
 * PURPOSE:
 *   Model lifecycle state for remote terminal with guarded transitions.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_TERMINAL_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_TERMINAL_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote remote terminal data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteRemoteTerminal { char id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; UmiTerminalRemoteState state; uint64_t revision; } UmiTerminalRemoteRemoteTerminal;
/**
 * Initialise terminal remote remote terminal from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_terminal_init(UmiTerminalRemoteRemoteTerminal *value,const char *id);
/**
 * Provide the terminal remote remote terminal transition operation used by this module and
 * its client applications.
 */
UmiStatus umi_terminal_remote_remote_terminal_transition(UmiTerminalRemoteRemoteTerminal *value,UmiTerminalRemoteState next);
/**
 * Provide the terminal remote remote terminal usable operation used by this module and its
 * client applications.
 */
bool umi_terminal_remote_remote_terminal_usable(const UmiTerminalRemoteRemoteTerminal *value);
#ifdef __cplusplus
}
#endif
#endif
