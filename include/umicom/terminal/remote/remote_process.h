/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_process.h
 *
 * PURPOSE:
 *   Model lifecycle state for remote process with guarded transitions.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_PROCESS_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_PROCESS_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote remote process data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteRemoteProcess { char id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; UmiTerminalRemoteState state; uint64_t revision; } UmiTerminalRemoteRemoteProcess;
/**
 * Initialise terminal remote remote process from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_process_init(UmiTerminalRemoteRemoteProcess *value,const char *id);
/**
 * Provide the terminal remote remote process transition operation used by this module and
 * its client applications.
 */
UmiStatus umi_terminal_remote_remote_process_transition(UmiTerminalRemoteRemoteProcess *value,UmiTerminalRemoteState next);
/**
 * Provide the terminal remote remote process usable operation used by this module and its
 * client applications.
 */
bool umi_terminal_remote_remote_process_usable(const UmiTerminalRemoteRemoteProcess *value);
#ifdef __cplusplus
}
#endif
#endif
