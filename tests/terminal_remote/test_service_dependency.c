/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_service_dependency.c
 *
 * PURPOSE:
 *   Verify required service dependencies gate startup.
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
#include "umicom/terminal/remote/service_dependency.h"
int main(void) { if(umi_terminal_remote_service_dependency_ready(3U,2U,false)) return 1; return umi_terminal_remote_service_dependency_ready(3U,3U,true)?0:2; }
