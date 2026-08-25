/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_tunnel.c
 *
 * PURPOSE:
 *   Verify tunnel readiness requires endpoint, authentication and non-critical health.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_tunnel.h"
int main(void) { if(!umi_terminal_remote_remote_tunnel_ready(true,true,UMI_TERMINAL_REMOTE_HEALTH_HEALTHY)) return 1; return umi_terminal_remote_remote_tunnel_ready(true,true,UMI_TERMINAL_REMOTE_HEALTH_CRITICAL)?2:0; }
