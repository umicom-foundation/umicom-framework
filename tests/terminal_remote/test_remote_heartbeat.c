/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_heartbeat.c
 *
 * PURPOSE:
 *   Verify remote heartbeat heartbeat warning and critical thresholds.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_heartbeat.h"
int main(void) { UmiTerminalRemoteRemoteHeartbeat v; umi_terminal_remote_remote_heartbeat_init(&v,100U,50U,100U); if(umi_terminal_remote_remote_heartbeat_health(&v,120U)!=UMI_TERMINAL_REMOTE_HEALTH_HEALTHY) return 1; if(umi_terminal_remote_remote_heartbeat_health(&v,170U)!=UMI_TERMINAL_REMOTE_HEALTH_WARNING) return 2; if(umi_terminal_remote_remote_heartbeat_health(&v,220U)!=UMI_TERMINAL_REMOTE_HEALTH_CRITICAL) return 3; umi_terminal_remote_remote_heartbeat_observe(&v,220U); return umi_terminal_remote_remote_heartbeat_health(&v,221U)==UMI_TERMINAL_REMOTE_HEALTH_HEALTHY?0:4; }
