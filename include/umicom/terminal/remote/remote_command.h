/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_command.h
 *
 * PURPOSE:
 *   Describe remote commands with explicit working-directory and interactive policy.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_COMMAND_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_COMMAND_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemoteCommand { char program[UMI_TERMINAL_REMOTE_PATH_CAPACITY]; char working_directory[UMI_TERMINAL_REMOTE_PATH_CAPACITY]; bool interactive; } UmiTerminalRemoteRemoteCommand;
void umi_terminal_remote_remote_command_init(UmiTerminalRemoteRemoteCommand *value,const char *program,const char *working_directory,bool interactive);
bool umi_terminal_remote_remote_command_valid(const UmiTerminalRemoteRemoteCommand *value);
#ifdef __cplusplus
}
#endif
#endif
