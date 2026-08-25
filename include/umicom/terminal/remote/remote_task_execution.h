/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_task_execution.h
 *
 * PURPOSE:
 *   Track remote task attempts and completion without product-owned execution state.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_TASK_EXECUTION_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_TASK_EXECUTION_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemoteTaskExecution { char task_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; uint32_t attempts; bool completed; int exit_code; } UmiTerminalRemoteRemoteTaskExecution;
void umi_terminal_remote_remote_task_execution_init(UmiTerminalRemoteRemoteTaskExecution *value,const char *task_id);
void umi_terminal_remote_remote_task_execution_start(UmiTerminalRemoteRemoteTaskExecution *value);
void umi_terminal_remote_remote_task_execution_complete(UmiTerminalRemoteRemoteTaskExecution *value,int exit_code);
bool umi_terminal_remote_remote_task_execution_successful(const UmiTerminalRemoteRemoteTaskExecution *value);
#ifdef __cplusplus
}
#endif
#endif
