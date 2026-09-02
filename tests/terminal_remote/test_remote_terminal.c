/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_terminal.c
 *
 * PURPOSE:
 *   Verify remote terminal lifecycle readiness and failed-state guard.
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
#include "umicom/terminal/remote/remote_terminal.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTerminalRemoteRemoteTerminal v; umi_terminal_remote_remote_terminal_init(&v,"x"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_terminal_remote_remote_terminal_usable(&v)) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_terminal_remote_remote_terminal_transition(&v,UMI_TERMINAL_REMOTE_STATE_FAILED)!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_terminal_remote_remote_terminal_transition(&v,UMI_TERMINAL_REMOTE_STATE_ACTIVE)!=UMI_STATUS_INVALID_STATE) return 3; return 0; }
