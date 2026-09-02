/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_workspace_mount.c
 *
 * PURPOSE:
 *   Implement remote workspace mount policy state.
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
#include <string.h>
/*
 * Initialise terminal remote remote workspace mount from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_workspace_mount_init(UmiTerminalRemoteRemoteWorkspaceMount *value,const char *root,bool read_only) { /* Apply this branch only when its contract condition is satisfied. */ if(!value) return; (void)memset(value,0,sizeof(*value)); /* Apply this branch only when its contract condition is satisfied. */ if(root) (void)umi_terminal_remote_copy_text(value->root,sizeof(value->root),root); value->read_only=read_only; }
/*
 * Write terminal remote remote workspace mount allows in its stable representation and
 * report capacity or input failures to the caller.
 */
bool umi_terminal_remote_remote_workspace_mount_allows_write(const UmiTerminalRemoteRemoteWorkspaceMount *value) { return value&&value->root[0]!='\0'&&!value->read_only; }
