/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_output_backpressure.c
 *
 * PURPOSE:
 *   Verify output backpressure applies high-water throttling and hard capacity.
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
#include "umicom/terminal/remote/terminal_output_backpressure.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTerminalRemoteTerminalOutputBackpressure v; umi_terminal_remote_terminal_output_backpressure_init(&v,100U,75U); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_terminal_remote_terminal_output_backpressure_enqueue(&v,80U)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_terminal_remote_terminal_output_backpressure_throttled(&v)) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_terminal_remote_terminal_output_backpressure_enqueue(&v,30U)!=UMI_STATUS_CAPACITY_EXCEEDED) return 3; umi_terminal_remote_terminal_output_backpressure_consume(&v,50U); return umi_terminal_remote_terminal_output_backpressure_throttled(&v)?4:0; }
