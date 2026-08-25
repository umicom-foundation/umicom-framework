/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_file_transfer.h
 *
 * PURPOSE:
 *   Track remote file-transfer progress with monotonic byte accounting.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_FILE_TRANSFER_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_FILE_TRANSFER_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemoteFileTransfer { uint64_t total_bytes; uint64_t transferred_bytes; } UmiTerminalRemoteRemoteFileTransfer;
void umi_terminal_remote_remote_file_transfer_init(UmiTerminalRemoteRemoteFileTransfer *value,uint64_t total_bytes);
UmiStatus umi_terminal_remote_remote_file_transfer_advance(UmiTerminalRemoteRemoteFileTransfer *value,uint64_t bytes);
uint32_t umi_terminal_remote_remote_file_transfer_percent(const UmiTerminalRemoteRemoteFileTransfer *value);
#ifdef __cplusplus
}
#endif
#endif
