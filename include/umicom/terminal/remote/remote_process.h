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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_PROCESS_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_PROCESS_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemoteProcess { char id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; UmiTerminalRemoteState state; uint64_t revision; } UmiTerminalRemoteRemoteProcess;
void umi_terminal_remote_remote_process_init(UmiTerminalRemoteRemoteProcess *value,const char *id);
UmiStatus umi_terminal_remote_remote_process_transition(UmiTerminalRemoteRemoteProcess *value,UmiTerminalRemoteState next);
bool umi_terminal_remote_remote_process_usable(const UmiTerminalRemoteRemoteProcess *value);
#ifdef __cplusplus
}
#endif
#endif
