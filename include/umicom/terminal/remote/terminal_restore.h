/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_restore.h
 *
 * PURPOSE:
 *   Model lifecycle state for terminal restore with guarded transitions.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_RESTORE_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_RESTORE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote terminal restore data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteTerminalRestore { char id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; UmiTerminalRemoteState state; uint64_t revision; } UmiTerminalRemoteTerminalRestore;
/**
 * Initialise terminal remote terminal restore from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_terminal_restore_init(UmiTerminalRemoteTerminalRestore *value,const char *id);
/**
 * Provide the terminal remote terminal restore transition operation used by this module
 * and its client applications.
 */
UmiStatus umi_terminal_remote_terminal_restore_transition(UmiTerminalRemoteTerminalRestore *value,UmiTerminalRemoteState next);
/**
 * Provide the terminal remote terminal restore usable operation used by this module and
 * its client applications.
 */
bool umi_terminal_remote_terminal_restore_usable(const UmiTerminalRemoteTerminalRestore *value);
#ifdef __cplusplus
}
#endif
#endif
