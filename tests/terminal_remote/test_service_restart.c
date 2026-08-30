/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_service_restart.c
 *
 * PURPOSE:
 *   Verify service restart restart attempt ceiling.
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
#include "umicom/terminal/remote/service_restart.h"
int main(void) { UmiTerminalRemoteServiceRestart v; umi_terminal_remote_service_restart_init(&v,2U); if(!umi_terminal_remote_service_restart_claim_attempt(&v)||!umi_terminal_remote_service_restart_claim_attempt(&v)) return 1; if(umi_terminal_remote_service_restart_claim_attempt(&v)) return 2; umi_terminal_remote_service_restart_reset(&v); return umi_terminal_remote_service_restart_claim_attempt(&v)?0:3; }
