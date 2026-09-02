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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_file_transfer.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTerminalRemoteRemoteFileTransfer v; umi_terminal_remote_remote_file_transfer_init(&v,200U); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_terminal_remote_remote_file_transfer_advance(&v,50U)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_terminal_remote_remote_file_transfer_percent(&v)!=25U) return 2; return umi_terminal_remote_remote_file_transfer_advance(&v,200U)==UMI_STATUS_CAPACITY_EXCEEDED?0:3; }
