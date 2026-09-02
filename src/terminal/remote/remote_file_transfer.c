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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_file_transfer.h"
/*
 * Initialise terminal remote remote file transfer from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_file_transfer_init(UmiTerminalRemoteRemoteFileTransfer *value,uint64_t total_bytes) { /* Apply this branch only when its contract condition is satisfied. */ if(value) { value->total_bytes=total_bytes; value->transferred_bytes=0U; } }
/*
 * Provide the terminal remote remote file transfer advance operation used by this module
 * and its client applications.
 */
UmiStatus umi_terminal_remote_remote_file_transfer_advance(UmiTerminalRemoteRemoteFileTransfer *value,uint64_t bytes) { /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value) return UMI_STATUS_INVALID_ARGUMENT; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(bytes>value->total_bytes-value->transferred_bytes) return UMI_STATUS_CAPACITY_EXCEEDED; value->transferred_bytes+=bytes; return UMI_STATUS_OK; }
/*
 * Provide the terminal remote remote file transfer percent operation used by this module
 * and its client applications.
 */
uint32_t umi_terminal_remote_remote_file_transfer_percent(const UmiTerminalRemoteRemoteFileTransfer *value) { /* Apply this branch only when its contract condition is satisfied. */ if(!value||value->total_bytes==0U) return 0U; return (uint32_t)((value->transferred_bytes*UINT64_C(100))/value->total_bytes); }
