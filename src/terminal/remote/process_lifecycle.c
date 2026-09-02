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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/process_lifecycle.h"
/*
 * Provide the terminal remote process lifecycle transition allowed operation used by this
 * module and its client applications.
 */
bool umi_terminal_remote_process_lifecycle_transition_allowed(UmiTerminalRemoteState from,UmiTerminalRemoteState to) { /* Preserve the original failure result so the caller can respond to the correct cause. */ if(to==UMI_TERMINAL_REMOTE_STATE_UNKNOWN) return false; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(from==UMI_TERMINAL_REMOTE_STATE_FAILED) return to==UMI_TERMINAL_REMOTE_STATE_STOPPED||to==UMI_TERMINAL_REMOTE_STATE_RECONNECTING; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(from==UMI_TERMINAL_REMOTE_STATE_STOPPED) return to==UMI_TERMINAL_REMOTE_STATE_READY; return true; }
