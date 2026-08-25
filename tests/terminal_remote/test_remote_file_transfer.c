/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_file_transfer.c
 *
 * PURPOSE:
 *   Verify remote transfer progress and overflow rejection.
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
int main(void) { UmiTerminalRemoteRemoteFileTransfer v; umi_terminal_remote_remote_file_transfer_init(&v,200U); if(umi_terminal_remote_remote_file_transfer_advance(&v,50U)!=UMI_STATUS_OK) return 1; if(umi_terminal_remote_remote_file_transfer_percent(&v)!=25U) return 2; return umi_terminal_remote_remote_file_transfer_advance(&v,200U)==UMI_STATUS_CAPACITY_EXCEEDED?0:3; }
