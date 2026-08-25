/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_file_transfer.c
 *
 * PURPOSE:
 *   Implement bounded remote transfer progress.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_file_transfer.h"
void umi_terminal_remote_remote_file_transfer_init(UmiTerminalRemoteRemoteFileTransfer *value,uint64_t total_bytes) { if(value) { value->total_bytes=total_bytes; value->transferred_bytes=0U; } }
UmiStatus umi_terminal_remote_remote_file_transfer_advance(UmiTerminalRemoteRemoteFileTransfer *value,uint64_t bytes) { if(!value) return UMI_STATUS_INVALID_ARGUMENT; if(bytes>value->total_bytes-value->transferred_bytes) return UMI_STATUS_CAPACITY_EXCEEDED; value->transferred_bytes+=bytes; return UMI_STATUS_OK; }
uint32_t umi_terminal_remote_remote_file_transfer_percent(const UmiTerminalRemoteRemoteFileTransfer *value) { if(!value||value->total_bytes==0U) return 0U; return (uint32_t)((value->transferred_bytes*UINT64_C(100))/value->total_bytes); }
