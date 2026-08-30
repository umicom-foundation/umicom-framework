/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_service_supervision.c
 *
 * PURPOSE:
 *   Verify critical active services request restart.
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
#include "umicom/terminal/remote/service_supervision.h"
int main(void) { return umi_terminal_remote_service_supervision_decide(UMI_TERMINAL_REMOTE_STATE_ACTIVE,UMI_TERMINAL_REMOTE_STATE_ACTIVE,UMI_TERMINAL_REMOTE_HEALTH_CRITICAL)==UMI_TERMINAL_REMOTE_SERVICE_SUPERVISION_RESTART?0:1; }
