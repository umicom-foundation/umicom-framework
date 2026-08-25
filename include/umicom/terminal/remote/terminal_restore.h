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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_RESTORE_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_RESTORE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteTerminalRestore { char id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; UmiTerminalRemoteState state; uint64_t revision; } UmiTerminalRemoteTerminalRestore;
void umi_terminal_remote_terminal_restore_init(UmiTerminalRemoteTerminalRestore *value,const char *id);
UmiStatus umi_terminal_remote_terminal_restore_transition(UmiTerminalRemoteTerminalRestore *value,UmiTerminalRemoteState next);
bool umi_terminal_remote_terminal_restore_usable(const UmiTerminalRemoteTerminalRestore *value);
#ifdef __cplusplus
}
#endif
#endif
