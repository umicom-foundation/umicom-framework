/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_file_operation.h
 *
 * PURPOSE:
 *   Authorize remote file operations against trust and mount write policy.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_FILE_OPERATION_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_FILE_OPERATION_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiTerminalRemoteRemoteFileOperationKind { UMI_TERMINAL_REMOTE_REMOTE_FILE_OPERATION_READ=1, UMI_TERMINAL_REMOTE_REMOTE_FILE_OPERATION_WRITE=2, UMI_TERMINAL_REMOTE_REMOTE_FILE_OPERATION_DELETE=3 } UmiTerminalRemoteRemoteFileOperationKind;
bool umi_terminal_remote_remote_file_operation_allowed(UmiTerminalRemoteRemoteFileOperationKind kind,bool trusted,bool read_only_mount);
#ifdef __cplusplus
}
#endif
#endif
