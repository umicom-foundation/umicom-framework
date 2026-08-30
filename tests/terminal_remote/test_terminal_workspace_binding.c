/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_workspace_binding.c
 *
 * PURPOSE:
 *   Verify terminal workspace binding requires two distinct stable identities.
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
#include "umicom/terminal/remote/terminal_workspace_binding.h"
int main(void) { UmiTerminalRemoteTerminalWorkspaceBinding v; umi_terminal_remote_terminal_workspace_binding_init(&v,"left","right"); if(!umi_terminal_remote_terminal_workspace_binding_valid(&v)) return 1; if(umi_terminal_remote_terminal_workspace_binding_fingerprint(&v)==0U) return 2; umi_terminal_remote_terminal_workspace_binding_init(&v,"same","same"); return umi_terminal_remote_terminal_workspace_binding_valid(&v)?3:0; }
