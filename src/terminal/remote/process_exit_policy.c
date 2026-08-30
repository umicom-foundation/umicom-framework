/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/process_exit_policy.c
 *
 * PURPOSE:
 *   Implement process exit classification and restart intent.
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
#include "umicom/terminal/remote/process_exit_policy.h"
void umi_terminal_remote_process_exit_policy_init(UmiTerminalRemoteProcessExitPolicy *value) { if(value) { value->success_code=0; value->restart_on_failure=true; } }
bool umi_terminal_remote_process_exit_policy_successful(const UmiTerminalRemoteProcessExitPolicy *value,int exit_code) { return value&&exit_code==value->success_code; }
bool umi_terminal_remote_process_exit_policy_restart(const UmiTerminalRemoteProcessExitPolicy *value,int exit_code) { return value&&value->restart_on_failure&&!umi_terminal_remote_process_exit_policy_successful(value,exit_code); }
