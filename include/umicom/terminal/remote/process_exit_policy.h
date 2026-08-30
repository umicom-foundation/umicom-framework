/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/process_exit_policy.h
 *
 * PURPOSE:
 *   Classify process exit codes for task/service supervision.
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
#ifndef UMICOM_TERMINAL_REMOTE_PROCESS_EXIT_POLICY_H
#define UMICOM_TERMINAL_REMOTE_PROCESS_EXIT_POLICY_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteProcessExitPolicy { int success_code; bool restart_on_failure; } UmiTerminalRemoteProcessExitPolicy;
void umi_terminal_remote_process_exit_policy_init(UmiTerminalRemoteProcessExitPolicy *value);
bool umi_terminal_remote_process_exit_policy_successful(const UmiTerminalRemoteProcessExitPolicy *value,int exit_code);
bool umi_terminal_remote_process_exit_policy_restart(const UmiTerminalRemoteProcessExitPolicy *value,int exit_code);
#ifdef __cplusplus
}
#endif
#endif
