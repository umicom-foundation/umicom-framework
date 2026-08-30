/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_capability.c
 *
 * PURPOSE:
 *   Verify remote capability capability subset and population count.
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
#include "umicom/terminal/remote/remote_capability.h"
int main(void) { if(!umi_terminal_remote_remote_capability_satisfies(UINT64_C(7),UINT64_C(3))) return 1; if(umi_terminal_remote_remote_capability_satisfies(UINT64_C(1),UINT64_C(3))) return 2; return umi_terminal_remote_remote_capability_count(UINT64_C(11))==3U?0:3; }
