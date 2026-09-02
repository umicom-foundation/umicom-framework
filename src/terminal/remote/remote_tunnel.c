/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_tunnel.c
 *
 * PURPOSE:
 *   Implement remote tunnel readiness aggregation.
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
#include "umicom/terminal/remote/remote_tunnel.h"
/*
 * Provide the terminal remote remote tunnel ready operation used by this module and its
 * client applications.
 */
bool umi_terminal_remote_remote_tunnel_ready(bool endpoint_ready,bool authenticated,UmiTerminalRemoteHealth health) { return endpoint_ready&&authenticated&&(health==UMI_TERMINAL_REMOTE_HEALTH_HEALTHY||health==UMI_TERMINAL_REMOTE_HEALTH_WARNING); }
