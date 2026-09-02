/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_process_restart_policy.c
 *
 * PURPOSE:
 *   Verify process restart policy restart attempt ceiling.
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
#include "umicom/terminal/remote/process_restart_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTerminalRemoteProcessRestartPolicy v; umi_terminal_remote_process_restart_policy_init(&v,2U); /* Apply this branch only when its contract condition is satisfied. */ if(!umi_terminal_remote_process_restart_policy_claim_attempt(&v)||!umi_terminal_remote_process_restart_policy_claim_attempt(&v)) return 1; /* Apply this branch only when its contract condition is satisfied. */ if(umi_terminal_remote_process_restart_policy_claim_attempt(&v)) return 2; umi_terminal_remote_process_restart_policy_reset(&v); return umi_terminal_remote_process_restart_policy_claim_attempt(&v)?0:3; }
