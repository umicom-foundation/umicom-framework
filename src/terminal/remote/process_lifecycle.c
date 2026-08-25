/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/process_lifecycle.c
 *
 * PURPOSE:
 *   Implement conservative lifecycle transition rules for process lifecycle.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/process_lifecycle.h"
bool umi_terminal_remote_process_lifecycle_transition_allowed(UmiTerminalRemoteState from,UmiTerminalRemoteState to) { if(to==UMI_TERMINAL_REMOTE_STATE_UNKNOWN) return false; if(from==UMI_TERMINAL_REMOTE_STATE_FAILED) return to==UMI_TERMINAL_REMOTE_STATE_STOPPED||to==UMI_TERMINAL_REMOTE_STATE_RECONNECTING; if(from==UMI_TERMINAL_REMOTE_STATE_STOPPED) return to==UMI_TERMINAL_REMOTE_STATE_READY; return true; }
