/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_background_service.c
 *
 * PURPOSE:
 *   Verify background service convergence only after observed state catches up.
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
#include "umicom/terminal/remote/background_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTerminalRemoteBackgroundService v; umi_terminal_remote_background_service_init(&v,"svc"); /* Apply this operation only while the related capability or state is available. */ if(umi_terminal_remote_background_service_converged(&v)) return 1; v.observed_state=UMI_TERMINAL_REMOTE_STATE_ACTIVE; return umi_terminal_remote_background_service_converged(&v)?0:2; }
