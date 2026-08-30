/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_workspace.c
 *
 * PURPOSE:
 *   Verify remote workspace bounded state and deterministic score.
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
#include "umicom/terminal/remote/remote_workspace.h"
int main(void) { UmiTerminalRemoteRemoteWorkspace v; umi_terminal_remote_remote_workspace_init(&v,"item",100U); if(umi_terminal_remote_remote_workspace_set_metric(&v,50U)!=UMI_STATUS_OK) return 1; if(!umi_terminal_remote_remote_workspace_within_limit(&v)||umi_terminal_remote_remote_workspace_score(&v)==0U) return 2; if(umi_terminal_remote_remote_workspace_set_metric(&v,101U)!=UMI_STATUS_OK) return 3; return umi_terminal_remote_remote_workspace_within_limit(&v)?4:0; }
