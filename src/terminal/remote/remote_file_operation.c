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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_file_operation.h"
/*
 * Provide the terminal remote remote file operation allowed operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_remote_file_operation_allowed(UmiTerminalRemoteRemoteFileOperationKind kind,bool trusted,bool read_only_mount) { /* Apply this operation only while the related capability or state is available. */ if(!trusted) return false; /* Apply this operation only while the related capability or state is available. */ if(kind==UMI_TERMINAL_REMOTE_REMOTE_FILE_OPERATION_READ) return true; return !read_only_mount&&(kind==UMI_TERMINAL_REMOTE_REMOTE_FILE_OPERATION_WRITE||kind==UMI_TERMINAL_REMOTE_REMOTE_FILE_OPERATION_DELETE); }
