/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/service_restart.c
 *
 * PURPOSE:
 *   Implement bounded restart claims for service restart.
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
#include "umicom/terminal/remote/service_restart.h"
/*
 * Initialise terminal remote service restart from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_service_restart_init(UmiTerminalRemoteServiceRestart *value,uint32_t max_attempts) { /* Apply this branch only when its contract condition is satisfied. */ if(value) { value->max_attempts=max_attempts; value->attempts=0U; } }
/*
 * Provide the terminal remote service restart claim attempt operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_service_restart_claim_attempt(UmiTerminalRemoteServiceRestart *value) { /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!value||value->attempts>=value->max_attempts) return false; value->attempts++; return true; }
/*
 * Release or reset state held by terminal remote service restart so the same storage can
 * be reused safely.
 */
void umi_terminal_remote_service_restart_reset(UmiTerminalRemoteServiceRestart *value) { /* Apply this branch only when its contract condition is satisfied. */ if(value) value->attempts=0U; }
