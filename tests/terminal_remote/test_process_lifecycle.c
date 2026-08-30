/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_process_lifecycle.c
 *
 * PURPOSE:
 *   Verify failed process lifecycle cannot jump directly to active.
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
#include "umicom/terminal/remote/process_lifecycle.h"
int main(void) { if(umi_terminal_remote_process_lifecycle_transition_allowed(UMI_TERMINAL_REMOTE_STATE_FAILED,UMI_TERMINAL_REMOTE_STATE_ACTIVE)) return 1; return umi_terminal_remote_process_lifecycle_transition_allowed(UMI_TERMINAL_REMOTE_STATE_FAILED,UMI_TERMINAL_REMOTE_STATE_RECONNECTING)?0:2; }
