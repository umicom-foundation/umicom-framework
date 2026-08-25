/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_process.c
 *
 * PURPOSE:
 *   Verify remote process lifecycle readiness and failed-state guard.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_process.h"
int main(void) { UmiTerminalRemoteRemoteProcess v; umi_terminal_remote_remote_process_init(&v,"x"); if(!umi_terminal_remote_remote_process_usable(&v)) return 1; if(umi_terminal_remote_remote_process_transition(&v,UMI_TERMINAL_REMOTE_STATE_FAILED)!=UMI_STATUS_OK) return 2; if(umi_terminal_remote_remote_process_transition(&v,UMI_TERMINAL_REMOTE_STATE_ACTIVE)!=UMI_STATUS_INVALID_STATE) return 3; return 0; }
