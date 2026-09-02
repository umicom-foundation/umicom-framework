/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_process_tree.c
 *
 * PURPOSE:
 *   Verify process ancestry across several generations.
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
#include "umicom/terminal/remote/process_tree.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTerminalRemoteProcessTree v; umi_terminal_remote_process_tree_init(&v); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_terminal_remote_process_tree_add(&v,10U,0U)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_terminal_remote_process_tree_add(&v,11U,10U)!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_terminal_remote_process_tree_add(&v,12U,11U)!=UMI_STATUS_OK) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_terminal_remote_process_tree_is_descendant(&v,12U,10U)) return 4; return umi_terminal_remote_process_tree_is_descendant(&v,10U,12U)?5:0; }
