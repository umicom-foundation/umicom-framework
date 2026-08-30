/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_resize.c
 *
 * PURPOSE:
 *   Verify terminal dimensions reject invalid or excessive geometry.
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
#include "umicom/terminal/remote/terminal_resize.h"
int main(void) { UmiTerminalRemoteTerminalResize v; umi_terminal_remote_terminal_resize_init(&v,80U,24U); if(umi_terminal_remote_terminal_resize_set(&v,120U,40U)!=UMI_STATUS_OK) return 1; return umi_terminal_remote_terminal_resize_set(&v,1U,40U)==UMI_STATUS_INVALID_ARGUMENT?0:2; }
