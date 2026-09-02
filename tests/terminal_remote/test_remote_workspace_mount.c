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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTerminalRemoteRemoteWorkspaceMount v; umi_terminal_remote_remote_workspace_mount_init(&v,"/work",true); /* Apply this branch only when its contract condition is satisfied. */ if(umi_terminal_remote_remote_workspace_mount_allows_write(&v)) return 1; umi_terminal_remote_remote_workspace_mount_init(&v,"/work",false); return umi_terminal_remote_remote_workspace_mount_allows_write(&v)?0:2; }
