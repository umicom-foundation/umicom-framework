/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_workspace_mount.c
 *
 * PURPOSE:
 *   Verify read-only remote mounts reject write intent.
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
#include "umicom/terminal/remote/remote_workspace_mount.h"
int main(void) { UmiTerminalRemoteRemoteWorkspaceMount v; umi_terminal_remote_remote_workspace_mount_init(&v,"/work",true); if(umi_terminal_remote_remote_workspace_mount_allows_write(&v)) return 1; umi_terminal_remote_remote_workspace_mount_init(&v,"/work",false); return umi_terminal_remote_remote_workspace_mount_allows_write(&v)?0:2; }
