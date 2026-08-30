/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_task_execution.c
 *
 * PURPOSE:
 *   Implement remote task attempt and completion state.
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
#include "umicom/terminal/remote/remote_task_execution.h"
#include <string.h>
void umi_terminal_remote_remote_task_execution_init(UmiTerminalRemoteRemoteTaskExecution *value,const char *task_id) { if(!value) return; (void)memset(value,0,sizeof(*value)); if(task_id) (void)umi_terminal_remote_copy_text(value->task_id,sizeof(value->task_id),task_id); }
void umi_terminal_remote_remote_task_execution_start(UmiTerminalRemoteRemoteTaskExecution *value) { if(value) { value->attempts++; value->completed=false; } }
void umi_terminal_remote_remote_task_execution_complete(UmiTerminalRemoteRemoteTaskExecution *value,int exit_code) { if(value) { value->exit_code=exit_code; value->completed=true; } }
bool umi_terminal_remote_remote_task_execution_successful(const UmiTerminalRemoteRemoteTaskExecution *value) { return value&&value->completed&&value->exit_code==0; }
