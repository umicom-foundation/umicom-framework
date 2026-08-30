/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_working_directory.h
 *
 * PURPOSE:
 *   Validate and update bounded filesystem path state for terminal working directory.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_WORKING_DIRECTORY_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_WORKING_DIRECTORY_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteTerminalWorkingDirectory { char path[UMI_TERMINAL_REMOTE_PATH_CAPACITY]; uint64_t revision; } UmiTerminalRemoteTerminalWorkingDirectory;
void umi_terminal_remote_terminal_working_directory_init(UmiTerminalRemoteTerminalWorkingDirectory *value);
UmiStatus umi_terminal_remote_terminal_working_directory_set(UmiTerminalRemoteTerminalWorkingDirectory *value,const char *path);
bool umi_terminal_remote_terminal_working_directory_absolute(const UmiTerminalRemoteTerminalWorkingDirectory *value);
#ifdef __cplusplus
}
#endif
#endif
