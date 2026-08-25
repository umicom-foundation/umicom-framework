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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_TERMINAL_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_TERMINAL_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemoteTerminal { char id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; UmiTerminalRemoteState state; uint64_t revision; } UmiTerminalRemoteRemoteTerminal;
void umi_terminal_remote_remote_terminal_init(UmiTerminalRemoteRemoteTerminal *value,const char *id);
UmiStatus umi_terminal_remote_remote_terminal_transition(UmiTerminalRemoteRemoteTerminal *value,UmiTerminalRemoteState next);
bool umi_terminal_remote_remote_terminal_usable(const UmiTerminalRemoteRemoteTerminal *value);
#ifdef __cplusplus
}
#endif
#endif
