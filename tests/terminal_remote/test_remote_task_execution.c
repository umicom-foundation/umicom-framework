/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_task_execution.c
 *
 * PURPOSE:
 *   Verify remote task execution records attempts and exit status.
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
#include "umicom/terminal/remote/remote_task_execution.h"
int main(void) { UmiTerminalRemoteRemoteTaskExecution v; umi_terminal_remote_remote_task_execution_init(&v,"build"); umi_terminal_remote_remote_task_execution_start(&v); umi_terminal_remote_remote_task_execution_complete(&v,0); if(v.attempts!=1U) return 1; return umi_terminal_remote_remote_task_execution_successful(&v)?0:2; }
