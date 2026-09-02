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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_command.h"
#include <string.h>
/*
 * Initialise terminal remote remote command from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_command_init(UmiTerminalRemoteRemoteCommand *value,const char *program,const char *working_directory,bool interactive) { /* Apply this operation only while the related capability or state is available. */ if(!value) return; (void)memset(value,0,sizeof(*value)); /* Apply this operation only while the related capability or state is available. */ if(program) (void)umi_terminal_remote_copy_text(value->program,sizeof(value->program),program); /* Apply this operation only while the related capability or state is available. */ if(working_directory) (void)umi_terminal_remote_copy_text(value->working_directory,sizeof(value->working_directory),working_directory); value->interactive=interactive; }
/*
 * Check that terminal remote remote command satisfies its contract before another service
 * relies on it.
 */
bool umi_terminal_remote_remote_command_valid(const UmiTerminalRemoteRemoteCommand *value) { return value&&value->program[0]!='\0'&&value->working_directory[0]!='\0'; }
