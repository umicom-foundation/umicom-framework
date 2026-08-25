/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_tunnel.h
 *
 * PURPOSE:
 *   Model secure remote tunnel readiness from endpoint/auth/heartbeat state.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_TUNNEL_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_TUNNEL_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
bool umi_terminal_remote_remote_tunnel_ready(bool endpoint_ready,bool authenticated,UmiTerminalRemoteHealth health);
#ifdef __cplusplus
}
#endif
#endif
