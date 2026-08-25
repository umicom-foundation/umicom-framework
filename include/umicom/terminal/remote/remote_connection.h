/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_connection.h
 *
 * PURPOSE:
 *   Model lifecycle state for remote connection with guarded transitions.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_CONNECTION_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_CONNECTION_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemoteConnection { char id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; UmiTerminalRemoteState state; uint64_t revision; } UmiTerminalRemoteRemoteConnection;
void umi_terminal_remote_remote_connection_init(UmiTerminalRemoteRemoteConnection *value,const char *id);
UmiStatus umi_terminal_remote_remote_connection_transition(UmiTerminalRemoteRemoteConnection *value,UmiTerminalRemoteState next);
bool umi_terminal_remote_remote_connection_usable(const UmiTerminalRemoteRemoteConnection *value);
#ifdef __cplusplus
}
#endif
#endif
