/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_session.c
 *
 * PURPOSE:
 *   Verify terminal session lifecycle readiness and failed-state guard.
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
#include "umicom/terminal/remote/terminal_session.h"
int main(void) { UmiTerminalRemoteTerminalSession v; umi_terminal_remote_terminal_session_init(&v,"x"); if(!umi_terminal_remote_terminal_session_usable(&v)) return 1; if(umi_terminal_remote_terminal_session_transition(&v,UMI_TERMINAL_REMOTE_STATE_FAILED)!=UMI_STATUS_OK) return 2; if(umi_terminal_remote_terminal_session_transition(&v,UMI_TERMINAL_REMOTE_STATE_ACTIVE)!=UMI_STATUS_INVALID_STATE) return 3; return 0; }
