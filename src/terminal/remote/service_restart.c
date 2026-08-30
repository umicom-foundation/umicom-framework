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
void umi_terminal_remote_service_restart_init(UmiTerminalRemoteServiceRestart *value,uint32_t max_attempts) { if(value) { value->max_attempts=max_attempts; value->attempts=0U; } }
bool umi_terminal_remote_service_restart_claim_attempt(UmiTerminalRemoteServiceRestart *value) { if(!value||value->attempts>=value->max_attempts) return false; value->attempts++; return true; }
void umi_terminal_remote_service_restart_reset(UmiTerminalRemoteServiceRestart *value) { if(value) value->attempts=0U; }
