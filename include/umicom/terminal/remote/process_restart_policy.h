/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/process_restart_policy.h
 *
 * PURPOSE:
 *   Bound restart attempts for process restart policy.
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
#ifndef UMICOM_TERMINAL_REMOTE_PROCESS_RESTART_POLICY_H
#define UMICOM_TERMINAL_REMOTE_PROCESS_RESTART_POLICY_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteProcessRestartPolicy { uint32_t max_attempts; uint32_t attempts; } UmiTerminalRemoteProcessRestartPolicy;
void umi_terminal_remote_process_restart_policy_init(UmiTerminalRemoteProcessRestartPolicy *value,uint32_t max_attempts);
bool umi_terminal_remote_process_restart_policy_claim_attempt(UmiTerminalRemoteProcessRestartPolicy *value);
void umi_terminal_remote_process_restart_policy_reset(UmiTerminalRemoteProcessRestartPolicy *value);
#ifdef __cplusplus
}
#endif
#endif
