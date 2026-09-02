/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_environment.c
 *
 * PURPOSE:
 *   Verify remote environment bounded state and deterministic score.
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
#include "umicom/terminal/remote/remote_environment.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTerminalRemoteRemoteEnvironment v; umi_terminal_remote_remote_environment_init(&v,"item",100U); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_terminal_remote_remote_environment_set_metric(&v,50U)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_terminal_remote_remote_environment_within_limit(&v)||umi_terminal_remote_remote_environment_score(&v)==0U) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_terminal_remote_remote_environment_set_metric(&v,101U)!=UMI_STATUS_OK) return 3; return umi_terminal_remote_remote_environment_within_limit(&v)?4:0; }
