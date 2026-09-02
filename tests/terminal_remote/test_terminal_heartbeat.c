/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_heartbeat.c
 *
 * PURPOSE:
 *   Verify terminal heartbeat heartbeat warning and critical thresholds.
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
#include "umicom/terminal/remote/terminal_heartbeat.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTerminalRemoteTerminalHeartbeat v; umi_terminal_remote_terminal_heartbeat_init(&v,100U,50U,100U); /* Apply this branch only when its contract condition is satisfied. */ if(umi_terminal_remote_terminal_heartbeat_health(&v,120U)!=UMI_TERMINAL_REMOTE_HEALTH_HEALTHY) return 1; /* Apply this branch only when its contract condition is satisfied. */ if(umi_terminal_remote_terminal_heartbeat_health(&v,170U)!=UMI_TERMINAL_REMOTE_HEALTH_WARNING) return 2; /* Apply this branch only when its contract condition is satisfied. */ if(umi_terminal_remote_terminal_heartbeat_health(&v,220U)!=UMI_TERMINAL_REMOTE_HEALTH_CRITICAL) return 3; umi_terminal_remote_terminal_heartbeat_observe(&v,220U); return umi_terminal_remote_terminal_heartbeat_health(&v,221U)==UMI_TERMINAL_REMOTE_HEALTH_HEALTHY?0:4; }
