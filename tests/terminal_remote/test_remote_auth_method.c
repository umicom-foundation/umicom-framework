/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_auth_method.c
 *
 * PURPOSE:
 *   Verify agent/key/password authentication risk ordering.
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
#include "umicom/terminal/remote/remote_auth_method.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { /* Apply this branch only when its contract condition is satisfied. */ if(!(umi_terminal_remote_remote_auth_method_risk(UMI_TERMINAL_REMOTE_REMOTE_AUTH_METHOD_AGENT,false)<umi_terminal_remote_remote_auth_method_risk(UMI_TERMINAL_REMOTE_REMOTE_AUTH_METHOD_PASSWORD,false))) return 1; return 0; }
