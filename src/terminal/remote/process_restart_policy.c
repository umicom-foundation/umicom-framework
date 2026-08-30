/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/process_restart_policy.c
 *
 * PURPOSE:
 *   Implement bounded restart claims for process restart policy.
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
#include "umicom/terminal/remote/process_restart_policy.h"
void umi_terminal_remote_process_restart_policy_init(UmiTerminalRemoteProcessRestartPolicy *value,uint32_t max_attempts) { if(value) { value->max_attempts=max_attempts; value->attempts=0U; } }
bool umi_terminal_remote_process_restart_policy_claim_attempt(UmiTerminalRemoteProcessRestartPolicy *value) { if(!value||value->attempts>=value->max_attempts) return false; value->attempts++; return true; }
void umi_terminal_remote_process_restart_policy_reset(UmiTerminalRemoteProcessRestartPolicy *value) { if(value) value->attempts=0U; }
