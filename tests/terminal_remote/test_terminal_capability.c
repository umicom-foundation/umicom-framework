/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_capability.c
 *
 * PURPOSE:
 *   Verify terminal capability capability subset and population count.
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
#include "umicom/terminal/remote/terminal_capability.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { /* Apply this branch only when its contract condition is satisfied. */ if(!umi_terminal_remote_terminal_capability_satisfies(UINT64_C(7),UINT64_C(3))) return 1; /* Apply this branch only when its contract condition is satisfied. */ if(umi_terminal_remote_terminal_capability_satisfies(UINT64_C(1),UINT64_C(3))) return 2; return umi_terminal_remote_terminal_capability_count(UINT64_C(11))==3U?0:3; }
