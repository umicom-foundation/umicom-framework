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
/*
 * Initialise terminal remote remote task execution from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_task_execution_init(UmiTerminalRemoteRemoteTaskExecution *value,const char *task_id) { /* Use the stable identifier comparison to choose the matching record or policy. */ if(!value) return; (void)memset(value,0,sizeof(*value)); /* Use the stable identifier comparison to choose the matching record or policy. */ if(task_id) (void)umi_terminal_remote_copy_text(value->task_id,sizeof(value->task_id),task_id); }
/*
 * Provide the terminal remote remote task execution start operation used by this module
 * and its client applications.
 */
void umi_terminal_remote_remote_task_execution_start(UmiTerminalRemoteRemoteTaskExecution *value) { /* Apply this branch only when its contract condition is satisfied. */ if(value) { value->attempts++; value->completed=false; } }
/*
 * Provide the terminal remote remote task execution complete operation used by this module
 * and its client applications.
 */
void umi_terminal_remote_remote_task_execution_complete(UmiTerminalRemoteRemoteTaskExecution *value,int exit_code) { /* Apply this branch only when its contract condition is satisfied. */ if(value) { value->exit_code=exit_code; value->completed=true; } }
/*
 * Provide the terminal remote remote task execution successful operation used by this
 * module and its client applications.
 */
bool umi_terminal_remote_remote_task_execution_successful(const UmiTerminalRemoteRemoteTaskExecution *value) { return value&&value->completed&&value->exit_code==0; }
