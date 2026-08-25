/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_file_operation.c
 *
 * PURPOSE:
 *   Implement remote file operation gate.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_file_operation.h"
bool umi_terminal_remote_remote_file_operation_allowed(UmiTerminalRemoteRemoteFileOperationKind kind,bool trusted,bool read_only_mount) { if(!trusted) return false; if(kind==UMI_TERMINAL_REMOTE_REMOTE_FILE_OPERATION_READ) return true; return !read_only_mount&&(kind==UMI_TERMINAL_REMOTE_REMOTE_FILE_OPERATION_WRITE||kind==UMI_TERMINAL_REMOTE_REMOTE_FILE_OPERATION_DELETE); }
