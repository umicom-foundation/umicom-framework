/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_command.c
 *
 * PURPOSE:
 *   Implement explicit remote command state without shell concatenation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_command.h"
#include <string.h>
void umi_terminal_remote_remote_command_init(UmiTerminalRemoteRemoteCommand *value,const char *program,const char *working_directory,bool interactive) { if(!value) return; (void)memset(value,0,sizeof(*value)); if(program) (void)umi_terminal_remote_copy_text(value->program,sizeof(value->program),program); if(working_directory) (void)umi_terminal_remote_copy_text(value->working_directory,sizeof(value->working_directory),working_directory); value->interactive=interactive; }
bool umi_terminal_remote_remote_command_valid(const UmiTerminalRemoteRemoteCommand *value) { return value&&value->program[0]!='\0'&&value->working_directory[0]!='\0'; }
