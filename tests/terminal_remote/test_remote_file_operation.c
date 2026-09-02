/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_file_operation.c
 *
 * PURPOSE:
 *   Verify remote file writes require trust and writable mount.
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
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { /* Apply this operation only while the related capability or state is available. */ if(!umi_terminal_remote_remote_file_operation_allowed(UMI_TERMINAL_REMOTE_REMOTE_FILE_OPERATION_READ,true,true)) return 1; /* Apply this operation only while the related capability or state is available. */ if(umi_terminal_remote_remote_file_operation_allowed(UMI_TERMINAL_REMOTE_REMOTE_FILE_OPERATION_WRITE,true,true)) return 2; return umi_terminal_remote_remote_file_operation_allowed(UMI_TERMINAL_REMOTE_REMOTE_FILE_OPERATION_WRITE,true,false)?0:3; }
