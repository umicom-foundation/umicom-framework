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
/*
 * Initialise terminal remote process restart policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_process_restart_policy_init(UmiTerminalRemoteProcessRestartPolicy *value,uint32_t max_attempts) { /* Apply this branch only when its contract condition is satisfied. */ if(value) { value->max_attempts=max_attempts; value->attempts=0U; } }
/*
 * Provide the terminal remote process restart policy claim attempt operation used by this
 * module and its client applications.
 */
bool umi_terminal_remote_process_restart_policy_claim_attempt(UmiTerminalRemoteProcessRestartPolicy *value) { /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!value||value->attempts>=value->max_attempts) return false; value->attempts++; return true; }
/*
 * Release or reset state held by terminal remote process restart policy so the same
 * storage can be reused safely.
 */
void umi_terminal_remote_process_restart_policy_reset(UmiTerminalRemoteProcessRestartPolicy *value) { /* Apply this branch only when its contract condition is satisfied. */ if(value) value->attempts=0U; }
