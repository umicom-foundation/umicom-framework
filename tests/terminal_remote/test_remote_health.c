/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_health.c
 *
 * PURPOSE:
 *   Verify remote health aggregates failures before latency.
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
#include "umicom/terminal/remote/remote_health.h"
int main(void) { if(umi_terminal_remote_remote_health_evaluate(0U,10U,100U)!=UMI_TERMINAL_REMOTE_HEALTH_HEALTHY) return 1; if(umi_terminal_remote_remote_health_evaluate(1U,10U,100U)!=UMI_TERMINAL_REMOTE_HEALTH_WARNING) return 2; return umi_terminal_remote_remote_health_evaluate(3U,1U,100U)==UMI_TERMINAL_REMOTE_HEALTH_CRITICAL?0:3; }
