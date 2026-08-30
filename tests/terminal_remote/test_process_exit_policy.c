/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_process_exit_policy.c
 *
 * PURPOSE:
 *   Verify process exit success and restart classification.
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
#include "umicom/terminal/remote/process_exit_policy.h"
int main(void) { UmiTerminalRemoteProcessExitPolicy v; umi_terminal_remote_process_exit_policy_init(&v); if(!umi_terminal_remote_process_exit_policy_successful(&v,0)) return 1; return umi_terminal_remote_process_exit_policy_restart(&v,2)?0:2; }
